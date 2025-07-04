/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#include "demultiplexer.hpp"

#include <cassert>

#include "demux/demultiplex_handler.hpp"
#include "demux/request_handler_holder.hpp"

namespace hutzn
{

demux_ptr make_demultiplexer(void)
{
    return std::make_shared<demultiplexer>();
}

demultiplexer::demultiplexer(void)
    : resource_callbacks_mutex_()
    , resource_callbacks_usage_changed_()
    , resource_callbacks_()
    , mime_handler_()
{
}

callback_holder_ptr demultiplexer::determine_request_handler(
    const request& request)
{
    callback_holder_ptr result;

    // a wildcard content type is not allowed
    const mime content_type = request.content_type();
    if ((content_type.first != mime_type::WILDCARD) &&
        (content_type.second != mime_subtype::WILDCARD)) {

        const char_t* const path = request.path();
        const http_verb method = request.method();

        // determine map of demanded resource
        std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);
        const resource_key key{path, method, content_type};
        const auto accept_it = resource_callbacks_.find(key);
        if (accept_it != resource_callbacks_.end()) {
            mime type;
            request_handler_callback callback;
            std::tie(type, callback) =
                lookup_in_accept_map(request, accept_it->second);
            if (!!callback) {
                request_handler_id id{path, method, content_type, type};
                result = std::make_shared<request_handler_holder>(*this, id,
                                                                  callback);
            }
        }
    }

    return result;
}

handler_ptr demultiplexer::connect(const request_handler_id& id,
                                   const request_handler_callback& fn)
{
    handler_ptr result;

    // check for invalid uri paths
    // check whether the content and accept mime is valid
    if (is_valid_url_path(id.path) &&
        mime_handler_.are_two_types_valid(id.content_type, id.accept_type)) {

        // get specific map with handlers
        std::unique_lock<std::mutex> cb_lock(resource_callbacks_mutex_);
        const resource_key key{id.path, id.method, id.content_type};
        auto& accept_map = resource_callbacks_[key];

        // check if there is already a registered request handler
        if (accept_map.insert(id.accept_type, fn)) {
            // the id have been inserted
            // there is no need to keep the lock during handler construction
            cb_lock.unlock();
            result = std::make_shared<demultiplex_handler>(*this, id);
        }
    }

    return result;
}

void demultiplexer::disconnect(const request_handler_id& id)
{
    std::unique_lock<std::mutex> lock(resource_callbacks_mutex_);

    bool repeat;
    do {
        // must reset the stop condition, because it gets only set true inside
        // the conditions
        repeat = false;

        // getting target resource map
        const resource_key key{id.path, id.method, id.content_type};
        auto resource_it = resource_callbacks_.find(key);
        if (resource_it != resource_callbacks_.end()) {
            demultiplexer_ordered_mime_map& accept_map = resource_it->second;

            // erase handler id
            if (!accept_map.erase(id.accept_type)) {
                repeat = true;
                resource_callbacks_usage_changed_.wait(lock);
            }

            // clean up empty maps
            if (accept_map.size() == 0) {
                const size_t erased_number = resource_callbacks_.erase(key);
                assert(erased_number == 1);
                UNUSED(erased_number);
            }
        }
    } while (repeat);
}

void demultiplexer::disable(const request_handler_id& id)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // getting target resource map
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // set the availability to true
        accept_map.set_availability(id.accept_type, false);
    }
}

void demultiplexer::enable(const request_handler_id& id)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // getting target resource map
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // set the availability to true
        accept_map.set_availability(id.accept_type, true);
    }
}

bool demultiplexer::is_enabled(const request_handler_id& id) const
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // getting target resource map
    bool result = false;
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        const demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // return the availability
        result = accept_map.is_available(id.accept_type);
    }
    return result;
}

mime_type demultiplexer::register_mime_type(const std::string& type)
{
    return mime_handler_.register_mime_type(type);
}

mime_subtype demultiplexer::register_mime_subtype(const std::string& subtype)
{
    return mime_handler_.register_mime_subtype(subtype);
}

bool demultiplexer::unregister_mime_type(const mime_type& type)
{
    return mime_handler_.unregister_mime_type(type);
}

bool demultiplexer::unregister_mime_subtype(const mime_subtype& subtype)
{
    return mime_handler_.unregister_mime_subtype(subtype);
}

void demultiplexer::increase_usage_counter(const request_handler_id& id)
{
    // no lock here, because we are called inside of determine_request_handler
    // and carry the lock already

    // getting target resource map
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // set the availability to true
        accept_map.increase_usage_counter(id.accept_type);
    }
}

void demultiplexer::decrease_usage_counter(const request_handler_id& id)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // getting target resource map
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it != resource_callbacks_.end()) {
        demultiplexer_ordered_mime_map& accept_map = resource_it->second;

        // set the availability to true
        accept_map.decrease_usage_counter(id.accept_type);

        // wake up potentially pending disconnect calls
        resource_callbacks_usage_changed_.notify_one();
    }
}

bool demultiplexer::resource_key::operator<(const resource_key& rhs) const
{
    return (path < rhs.path) || (method < rhs.method) ||
           (content_type < rhs.content_type);
}

std::tuple<mime, request_handler_callback> demultiplexer::lookup_in_accept_map(
    const request& request,
    const demultiplexer_ordered_mime_map& accept_map) const
{
    std::tuple<mime, request_handler_callback> result;

    // loop over the accept types for a matching request handler
    void* handle = NULL;
    mime type;
    while ((!std::get<1>(result)) && request.accept(handle, type)) {
        request_handler_callback callback = accept_map.find(type);
        result = std::make_tuple(type, callback);
    }

    return result;
}

} // namespace hutzn
