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

#ifndef LIBHUTZNOHMD_DEMUX_DEMULTIPLEX_HANDLER_HPP
#define LIBHUTZNOHMD_DEMUX_DEMULTIPLEX_HANDLER_HPP

#include <memory>

#include <libhutznohmd/demux.hpp>

#include <demux/disconnect_interface.hpp>

namespace hutzn
{

//! @brief Serves as handle for a registered request handler.
//!
//! It is created when a request handler gets registered and unregisters it
//! on destruction. It is required to release all demultiplex handlers before
//! destroying the demultiplexer.
class demultiplex_handler : public handler_interface
{
public:
    //! @brief Connects the request handler with the demultiplexer's disconnect
    //! interface.
    explicit demultiplex_handler(disconnect_interface& demuxer,
                                 const request_handler_id& id);

    //! @brief Disconnects the request handler from the demultiplexer.
    //!
    //! Ensure, that the demultiplexer is still existing when destroying an
    //! instance of a demultiplex handler.
    ~demultiplex_handler(void) noexcept(true) override;

private:
    //! References the demultiplexer where the request handler was registered
    //! at.
    disconnect_interface& demuxer_;

    //! This id is needed as key to disconnect the request handler.
    request_handler_id id_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEX_HANDLER_HPP
