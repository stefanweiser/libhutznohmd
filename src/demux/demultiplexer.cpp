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

#include <demux/demultiplex_handler.hpp>

#include "demultiplexer.hpp"

namespace hutzn
{

demux_pointer make_demultiplexer()
{
    return std::make_shared<demultiplexer>();
}

demultiplexer::demultiplexer()
    : resource_callbacks_mutex_()
    , resource_callbacks_()
    , request_parser_data_mutex_()
    , request_parser_data_()
{
}

request_handler_callback demultiplexer::determine_request_handler(
    const request_interface& request)
{
    // A wildcard content type is not allowed.
    const mime content_type = request.content_type();
    if ((content_type.first == mime_type::WILDCARD) ||
        (content_type.second == mime_subtype::WILDCARD)) {
        return request_handler_callback();
    }

    const char* const path = request.path();
    const http_verb method = request.method();

    // Determine map of demanded resource.
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);
    const resource_key key{path, method, content_type};
    const auto accept_it = resource_callbacks_.find(key);
    if (accept_it == resource_callbacks_.end()) {
        return request_handler_callback();
    }
    const demultiplexer_ordered_mime_map& accept_map = accept_it->second;

    // Loop over the accept types for a matching request handler.
    void* handle = nullptr;
    mime type;
    while (true == request.accept(handle, type)) {
        request_handler_callback result = accept_map.find(type);
        if (!!result) {
            return result;
        }
    }
    return request_handler_callback();
}

handler_pointer demultiplexer::connect(const request_handler_id& id,
                                       const request_handler_callback& fn)
{
    // Check for invalid uri paths.
    if (false == is_valid_uri_path(id.path)) {
        return handler_pointer();
    }

    {
        // Check whether the content and accept mime is valid.
        std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
        if ((false == is_mime_valid(id.content_type)) ||
            (false == is_mime_valid(id.accept_type))) {
            return handler_pointer();
        }
    }

    // Get specific map with handlers.
    std::unique_lock<std::mutex> lock(resource_callbacks_mutex_);
    const resource_key key{id.path, id.method, id.content_type};
    auto& accept_map = resource_callbacks_[key];

    // Check if there is already a registered request handler.
    if (true == accept_map.insert(id.accept_type, fn)) {
        // The id have been inserted. There is no need to keep the lock, when
        // creating the resulting handler.
        lock.unlock();
        return std::make_shared<demultiplex_handler>(*this, id);
    }
    return handler_pointer();
}

bool demultiplexer::disconnect(const request_handler_id& id)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // Getting target resource map.
    const resource_key key{id.path, id.method, id.content_type};
    auto resource_it = resource_callbacks_.find(key);
    if (resource_it == resource_callbacks_.end()) {
        return false;
    }
    demultiplexer_ordered_mime_map& accept_map = resource_it->second;

    // Erase handler id.
    const bool result = accept_map.erase(id.accept_type);

    // Clean up empty maps.
    if (accept_map.size() == 0) {
        resource_callbacks_.erase(key);
    }

    return result;
}

mime_type demultiplexer::register_mime_type(const std::string& type)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_.register_mime_type(type);
}

mime_subtype demultiplexer::register_mime_subtype(const std::string& subtype)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_.register_mime_subtype(subtype);
}

bool demultiplexer::unregister_mime_type(const mime_type& type)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_.unregister_mime_type(type);
}

bool demultiplexer::unregister_mime_subtype(const mime_subtype& subtype)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_.unregister_mime_subtype(subtype);
}

bool demultiplexer::is_mime_valid(const mime& t) const
{
    const mime_type type = t.first;
    const mime_subtype subtype = t.second;

    // Either type and subtype is not unset or both are unset.
    if (((type == mime_type::NONE) && (subtype != mime_subtype::NONE)) ||
        ((type != mime_type::NONE) && (subtype == mime_subtype::NONE))) {
        return false;
    }

    // Of course invalid values are not valid.
    if ((type == mime_type::INVALID) || (subtype == mime_subtype::INVALID)) {
        return false;
    }

    // Valid values must be registered. Unset types are unregistered but also
    // valid.
    if ((false == request_parser_data_.is_mime_type_registered(type)) ||
        (false == request_parser_data_.is_mime_subtype_registered(subtype))) {
        if ((type != mime_type::NONE) && (subtype != mime_subtype::NONE)) {
            return false;
        }
    }

    return true;
}

} // namespace hutzn
