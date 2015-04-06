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

namespace demux
{

demux_pointer make_demultiplexer()
{
    return std::make_shared<demultiplexer>();
}

demultiplexer::demultiplexer()
    : resource_callbacks_mutex_()
    , resource_callbacks_()
    , request_parser_data_mutex_()
    , request_parser_data_(std::make_shared<hutzn::request::parser_data>())
{
}

request_handler_callback demultiplexer::determine_request_handler(
    const hutzn::request::request_interface& /*request*/)
{
    return request_handler_callback();
}

handler_pointer demultiplexer::connect(const request_handler_id& id,
                                       const request_handler_callback& fn)
{
    std::lock_guard<std::mutex> lock(resource_callbacks_mutex_);
    resource_mime_map& mime_map = resource_callbacks_[id.path][id.method];
    auto mime_tuple = std::make_tuple(id.input_type, id.result_type);
    auto it = mime_map.find(mime_tuple);
    if (it == mime_map.end()) {
        mime_map[mime_tuple] = fn;
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

hutzn::request::mime_type demultiplexer::register_mime_type(
    const std::string& type)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->register_mime_type(type);
}

hutzn::request::mime_subtype demultiplexer::register_mime_subtype(
    const std::string& subtype)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->register_mime_subtype(subtype);
}

bool demultiplexer::unregister_mime_type(const hutzn::request::mime_type& type)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->unregister_mime_type(type);
}

bool demultiplexer::unregister_mime_subtype(
    const hutzn::request::mime_subtype& subtype)
{
    std::lock_guard<std::mutex> lock(request_parser_data_mutex_);
    return request_parser_data_->unregister_mime_subtype(subtype);
}

} // namespace demux

} // namespace hutzn
