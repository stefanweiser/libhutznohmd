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

#ifndef LIBHUTZNOHMD_DEMUX_DEMUX_DISCONNECT_INTERFACE_HPP
#define LIBHUTZNOHMD_DEMUX_DEMUX_DISCONNECT_INTERFACE_HPP

#include <libhutznohmd/demux.hpp>

namespace hutzn
{

//! When connecting a request handler to a demultiplexer, you'll need to
//! disconnect it later. This is done by releasing the handler. The handler in
//! turn will disconnect the request handler from its demultiplexer. This is an
//! internal interface.
class demux_disconnect_interface
{
public:
    virtual ~demux_disconnect_interface();

    //! Disconnects the handler with the given id from the demultiplexer.
    //! Returns true when the disconnect was successful and false when the
    //! handler with the given id could not be found.
    virtual bool disconnect(const request_handler_id& id) = 0;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMUX_DISCONNECT_INTERFACE_HPP
