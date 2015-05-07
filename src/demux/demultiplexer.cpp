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
    , request_parser_data_(std::make_shared<hutzn::parser_data>())
{
}

request_handler_callback demultiplexer::determine_request_handler(
    const hutzn::request_interface& request)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);

    const char* const path = request.path();
    const auto resource_it = resource_callbacks_.find(path);
    if (resource_it == resource_callbacks_.end()) {
        return request_handler_callback();
    }
    const auto& resource_map = resource_it->second;

    const hutzn::http_verb verb = request.method();
    const auto method_it = resource_map.find(verb);
    if (method_it == resource_map.end()) {
        return request_handler_callback();
    }
    const auto& method_map = method_it->second;

    const hutzn::mime content_type = request.content_type();
    if ((content_type.first == hutzn::mime_type::WILDCARD) ||
        (content_type.second == hutzn::mime_subtype::WILDCARD)) {
        return request_handler_callback();
    }

    void* handle = nullptr;
    hutzn::mime accept_type;
    while (true == request.accept(handle, accept_type)) {
        const auto handler_it =
            method_map.find(std::make_tuple(content_type, accept_type));
        if (handler_it != method_map.end()) {
            return handler_it->second;
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

    std::unique_lock<std::mutex> lock(resource_callbacks_mutex_);

    // Check whether the input mime type or subtype is unregistered.
    hutzn::mime_type type = id.input_type.first;
    hutzn::mime_subtype subtype = id.input_type.second;
    if ((false == request_parser_data_->is_mime_type_registered(type)) ||
        (false == request_parser_data_->is_mime_subtype_registered(subtype))) {
        return handler_pointer();
    }

    // Check whether the result mime type or subtype is unregistered.
    type = id.result_type.first;
    subtype = id.result_type.second;
    if ((false == request_parser_data_->is_mime_type_registered(type)) ||
        (false == request_parser_data_->is_mime_subtype_registered(subtype))) {
        return handler_pointer();
    }

    resource_mime_map& mime_map = resource_callbacks_[id.path][id.method];
    auto mime_tuple = std::make_tuple(id.input_type, id.result_type);
    auto it = mime_map.find(mime_tuple);
    if (it == mime_map.end()) {
        mime_map[mime_tuple] = fn;

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
    resource_mime_map& mime_map = resource_callbacks_[id.path][id.method];
    auto mime_tuple = std::make_tuple(id.input_type, id.result_type);
    const bool result = (mime_map.erase(mime_tuple) > 0);

    // Clean up empty maps.
    if (mime_map.size() == 0) {
        resource_callbacks_[id.path].erase(id.method);
        if (resource_callbacks_[id.path].size() == 0) {
            resource_callbacks_.erase(id.path);
        }
    }
    return result;
}

hutzn::mime_type demultiplexer::register_mime_type(const std::string& type)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->register_mime_type(type);
}

hutzn::mime_subtype demultiplexer::register_mime_subtype(
    const std::string& subtype)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->register_mime_subtype(subtype);
}

bool demultiplexer::unregister_mime_type(const hutzn::mime_type& type)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->unregister_mime_type(type);
}

bool demultiplexer::unregister_mime_subtype(const hutzn::mime_subtype& subtype)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->unregister_mime_subtype(subtype);
}

} // namespace hutzn
