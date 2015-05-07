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
    , request_parser_data_(std::make_shared<parser_data>())
{
}

request_handler_callback demultiplexer::determine_request_handler(
    const request_interface& request)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    // Determine map of demanded resource.
    const char* const path = request.path();
    const auto resource_it = resource_callbacks_.find(path);
    if (resource_it == resource_callbacks_.end()) {
        return request_handler_callback();
    }
    const auto& method_map = resource_it->second;

    // Determine map of demanded http method.
    const http_verb verb = request.method();
    const auto method_it = method_map.find(verb);
    if (method_it == method_map.end()) {
        return request_handler_callback();
    }
    const auto& content_map = method_it->second;

    // A wildcard content type is not allowed.
    const mime content_type = request.content_type();
    if ((content_type.first == mime_type::WILDCARD) ||
        (content_type.second == mime_subtype::WILDCARD)) {
        return request_handler_callback();
    }

    // Determine map of demanded content type.
    const auto content_it = content_map.find(content_type);
    if (content_it == content_map.end()) {
        return request_handler_callback();
    }
    const auto& accept_map = content_it->second;

    // Loop over the accept types for a matching request handler.
    return accept_map.find(request);
}

handler_pointer demultiplexer::connect(const request_handler_id& id,
                                       const request_handler_callback& fn)
{
    // Check for invalid uri paths.
    if (false == is_valid_uri_path(id.path)) {
        return handler_pointer();
    }

    std::unique_lock<std::mutex> lock(resource_callbacks_mutex_);

    // Check whether the input mime type or subtype is unregistered.
    mime_type type = id.content_type.first;
    mime_subtype subtype = id.content_type.second;
    if ((false == request_parser_data_->is_mime_type_registered(type)) ||
        (false == request_parser_data_->is_mime_subtype_registered(subtype))) {
        if ((type != mime_type::NONE) && (subtype != mime_subtype::NONE)) {
            return handler_pointer();
        }
    }

    // Check whether the result mime type or subtype is unregistered.
    type = id.accept_type.first;
    subtype = id.accept_type.second;
    if ((false == request_parser_data_->is_mime_type_registered(type)) ||
        (false == request_parser_data_->is_mime_subtype_registered(subtype))) {
        if ((type != mime_type::NONE) && (subtype != mime_subtype::NONE)) {
            return handler_pointer();
        }
    }

    // Get specific map with handlers.
    auto& accept_map = resource_callbacks_[id.path][id.method][id.content_type];

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
    auto resource_it = resource_callbacks_.find(id.path);
    if (resource_it == resource_callbacks_.end()) {
        return false;
    }
    auto& resource_map = resource_it->second;

    // Getting target method map.
    auto method_it = resource_map.find(id.method);
    if (method_it == resource_map.end()) {
        return false;
    }
    auto& method_map = method_it->second;

    // Getting target accept map.
    auto accept_it = method_map.find(id.content_type);
    if (accept_it == method_map.end()) {
        return false;
    }
    auto& accept_map = accept_it->second;

    // Erase handler id.
    const bool result = accept_map.erase(id.accept_type);

    // Clean up empty maps.
    if (accept_map.size() == 0) {
        method_map.erase(id.content_type);
        if (method_map.size() == 0) {
            resource_map.erase(id.method);
            if (resource_map.size() == 0) {
                resource_callbacks_.erase(id.path);
            }
        }
    }

    return result;
}

mime_type demultiplexer::register_mime_type(const std::string& type)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->register_mime_type(type);
}

mime_subtype demultiplexer::register_mime_subtype(const std::string& subtype)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->register_mime_subtype(subtype);
}

bool demultiplexer::unregister_mime_type(const mime_type& type)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->unregister_mime_type(type);
}

bool demultiplexer::unregister_mime_subtype(const mime_subtype& subtype)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->unregister_mime_subtype(subtype);
}

} // namespace hutzn
