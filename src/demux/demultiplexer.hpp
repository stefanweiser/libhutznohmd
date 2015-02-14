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

#ifndef LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_HPP
#define LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_HPP

#include <libhutznohmd/demux.hpp>

namespace hutzn
{

namespace demux
{

class demultiplexer : public demux_interface
{
public:
    virtual request_handler_callback
    determine_request_handler(const hutzn::request::request_interface& request);

    virtual handler_pointer connect(const request_handler_id& id,
                                    const request_handler_callback& fn);

    virtual bool register_mime_type(const std::string& type,
                                    hutzn::request::mime_type& result);

    virtual bool register_mime_subtype(const std::string& subtype,
                                       hutzn::request::mime_subtype& result);

    virtual bool unregister_mime_type(const hutzn::request::mime_type& type);

    virtual bool
    unregister_mime_subtype(const hutzn::request::mime_subtype& subtype);
};

} // namespace demux

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_HPP
