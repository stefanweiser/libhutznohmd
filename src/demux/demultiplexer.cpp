/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

 * The libhutznohmd project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The libhutznohmd project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the libhutznohmd project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <cassert>

#include <demux/demultiplex_handler.hpp>
#include <demux/request_handler_holder.hpp>

#include "demultiplexer.hpp"

namespace hutzn
{

demux_pointer make_demultiplexer(void)
{
    return std::make_shared<demultiplexer>();
}

demultiplexer::demultiplexer(void)
    : resource_callbacks_mutex_()
    , resource_callbacks_usage_changed_()
    , resource_callbacks_()
    , mime_type_mutex_()
    , mime_types_()
    , mime_subtypes_()
{
}

request_handler_holder_pointer demultiplexer::determine_request_handler(
    const request_interface& request)
{
    request_handler_holder_pointer result;

    // A wildcard content type is not allowed.
    const mime content_type = request.content_type();
    if ((content_type.first != mime_type::WILDCARD) &&
        (content_type.second != mime_subtype::WILDCARD)) {

        const char_t* const path = request.path();
        const http_verb method = request.method();

        // Determine map of demanded resource.
        std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);
        const resource_key key{path, method, content_type};
        const auto accept_it = resource_callbacks_.find(key);
        if (accept_it != resource_callbacks_.end()) {

            const demultiplexer_ordered_mime_map& accept_map =
                accept_it->second;

            // Loop over the accept types for a matching request handler.
            void* handle = nullptr;
            mime type;
            while (true == request.accept(handle, type)) {
                request_handler_callback find_result = accept_map.find(type);
                if (!!find_result) {
                    request_handler_id id{path, method, content_type, type};
                    result = std::make_shared<request_handler_holder>(
                        *this, id, find_result);
                    break;
                }
            }
        }
    }

    return result;
}

handler_pointer demultiplexer::connect(const request_handler_id& id,
                                       const request_handler_callback& fn)
{
    handler_pointer result;

    // Check for invalid uri paths.
    if (true == is_valid_uri_path(id.path)) {

        // Check whether the content and accept mime is valid.
        std::unique_lock<std::mutex> lock(mime_type_mutex_);
        if ((true == is_mime_valid(id.content_type)) &&
            (true == is_mime_valid(id.accept_type))) {

            lock.unlock();

            // Get specific map with handlers.
            std::unique_lock<std::mutex> cb_lock(resource_callbacks_mutex_);
            const resource_key key{id.path, id.method, id.content_type};
            auto& accept_map = resource_callbacks_[key];

            // Check if there is already a registered request handler.
            if (true == accept_map.insert(id.accept_type, fn)) {
                // The id have been inserted. There is no need to keep the lock,
                // when creating the resulting handler.
                cb_lock.unlock();
                result = std::make_shared<demultiplex_handler>(*this, id);
            }
        }
    }

    return result;
}

void demultiplexer::disconnect(const request_handler_id& id)
{
    std::unique_lock<std::mutex> lock(resource_callbacks_mutex_);

    bool repeat = false;
    do {
        // Getting target resource map.
        const resource_key key{id.path, id.method, id.content_type};
        auto resource_it = resource_callbacks_.find(key);
        if (resource_it != resource_callbacks_.end()) {
            demultiplexer_ordered_mime_map& accept_map = resource_it->second;

            // Erase handler id.
            if (false == accept_map.erase(id.accept_type)) {
                repeat = true;
                resource_callbacks_usage_changed_.wait(lock);
            }

            // Clean up empty maps.
            if (accept_map.size() == 0) {
                const size_t erased_number = resource_callbacks_.erase(key);
                assert(erased_number == 1);
                UNUSED(erased_number);
            }
        }
    } while (true == repeat);
}

void demultiplexer::disable(const request_handler_id& id)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // Getting target resource map.
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // Set the availability to false.
        accept_map.set_availability(id.accept_type, false);
    }
}

void demultiplexer::enable(const request_handler_id& id)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // Getting target resource map.
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // Set the availability to true.
        accept_map.set_availability(id.accept_type, true);
    }
}

bool demultiplexer::is_enabled(const request_handler_id& id) const
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // Getting target resource map.
    bool result = false;
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        const demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // Return the availability.
        result = accept_map.is_available(id.accept_type);
    }
    return result;
}

mime_type demultiplexer::register_mime_type(const std::string& type)
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_types_.register_type(type);
}

mime_subtype demultiplexer::register_mime_subtype(const std::string& subtype)
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_subtypes_.register_type(subtype);
}

bool demultiplexer::unregister_mime_type(const mime_type& type)
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_types_.unregister_type(type);
}

bool demultiplexer::unregister_mime_subtype(const mime_subtype& subtype)
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_subtypes_.unregister_type(subtype);
}

void demultiplexer::increase_usage_counter(const request_handler_id& id)
{
    // No lock here, because we are called inside of determine_request_handler
    // and carry the lock already.

    // Getting target resource map.
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // Set the availability to true.
        accept_map.increase_usage_counter(id.accept_type);
    }
}

void demultiplexer::decrease_usage_counter(const request_handler_id& id)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // Getting target resource map.
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // Set the availability to true.
        accept_map.decrease_usage_counter(id.accept_type);

        // Wake up potentially pending disconnect calls.
        resource_callbacks_usage_changed_.notify_one();
    }
}

bool demultiplexer::is_mime_valid(const mime& t) const
{
    const mime_type type = t.first;
    const mime_subtype subtype = t.second;

    bool result = true;

    // Either type and subtype is not unset or both are unset.
    if (((type == mime_type::NONE) && (subtype != mime_subtype::NONE)) ||
        ((type != mime_type::NONE) && (subtype == mime_subtype::NONE))) {
        result = false;
    }

    // Of course invalid values are not valid.
    if ((type == mime_type::INVALID) || (subtype == mime_subtype::INVALID)) {
        result = false;
    }

    // Valid values must be registered. Unset types are unregistered but also
    // valid.
    if ((false == mime_types_.is_registered(type)) ||
        (false == mime_subtypes_.is_registered(subtype))) {
        if ((type != mime_type::NONE) && (subtype != mime_subtype::NONE)) {
            result = false;
        }
    }

    return result;
}

bool demultiplexer::resource_key::operator<(const resource_key& rhs) const
{
    return ((path < rhs.path) || (method < rhs.method) ||
            (content_type < rhs.content_type));
}

} // namespace hutzn
