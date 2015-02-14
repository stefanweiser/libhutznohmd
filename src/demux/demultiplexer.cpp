/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

namespace hutzn
{

namespace demux
{

demux_pointer make_demultiplexer()
{
    return std::make_shared<demultiplexer>();
}

demultiplexer::demultiplexer()
    : request_parser_data_(std::make_shared<hutzn::request::parser_data>())
{
}

request_handler_callback demultiplexer::determine_request_handler(
    const hutzn::request::request_interface& /*request*/)
{
    return request_handler_callback();
}

handler_pointer demultiplexer::connect(const request_handler_id& /*id*/,
                                       const request_handler_callback& /*fn*/)
{
    return handler_pointer();
}

hutzn::request::mime_type
demultiplexer::register_mime_type(const std::string& type)
{
    return request_parser_data_->register_mime_type(type);
}

hutzn::request::mime_subtype
demultiplexer::register_mime_subtype(const std::string& subtype)
{
    return request_parser_data_->register_mime_subtype(subtype);
}

bool demultiplexer::unregister_mime_type(const hutzn::request::mime_type& type)
{
    return request_parser_data_->unregister_mime_type(type);
}

bool demultiplexer::unregister_mime_subtype(
    const hutzn::request::mime_subtype& subtype)
{
    return request_parser_data_->unregister_mime_subtype(subtype);
}

std::shared_ptr<hutzn::request::parser_data>
demultiplexer::request_parser_data() const
{
    return request_parser_data_;
}

} // namespace demux

} // namespace hutzn
