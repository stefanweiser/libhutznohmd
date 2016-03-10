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

#ifndef LIBHUTZNOHMD_DEMUX_HANDLER_MANAGER_HPP
#define LIBHUTZNOHMD_DEMUX_HANDLER_MANAGER_HPP

#include "libhutznohmd/demux.hpp"

namespace hutzn
{

//! @brief Provides a standardized way to manage a request handler on a
//! demultiplexer.
//!
//! When connecting a request handler to a demultiplexer, it has to be
//! disconnected later. This is done by releasing the handler. The handler in
//! turn will disconnect the request handler from its demultiplexer. This is an
//! internal interface.
class handler_manager
{
public:
    //! @brief Destroys a disconnect interface.
    virtual ~handler_manager(void) noexcept(true);

    //! @brief Disconnects the handler with the given id from the demultiplexer.
    virtual void disconnect(const request_handler_id& id) = 0;

    //! @brief Disables all calls to this request handler.
    //!
    //! After disabling the handler, it cannot be used. An error handler is then
    //! called instead.
    virtual void disable(const request_handler_id& id) = 0;

    //! Enables all calls to this request handler.
    virtual void enable(const request_handler_id& id) = 0;

    //! Returns true when the request handler is enabled and false, if not.
    virtual bool is_enabled(const request_handler_id& id) const = 0;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_HANDLER_MANAGER_HPP
