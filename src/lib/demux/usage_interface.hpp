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

#ifndef LIBHUTZNOHMD_DEMUX_USAGE_INTERFACE_HPP
#define LIBHUTZNOHMD_DEMUX_USAGE_INTERFACE_HPP

#include "libhutznohmd/demux.hpp"

namespace hutzn
{

//! @brief Adds the ability to control the usage flag of a request handler.
//!
//! When a request handler is determined to be used to answer a request, this
//! request handler has to be set used to signalize, that erasing the handler
//! is currently prohibited. When the call to handler is finished it gets
//! unused.
class usage_interface
{
public:
    virtual ~usage_interface(void);

    //! Sets the handler with the given id as used. Note, that this method is
    //! not thread-safe, because it has to be solely called during determining
    //! the request handler!
    virtual void increase_usage_counter(const request_handler_id& id) = 0;

    //! Sets the handler with the given id as unused.
    virtual void decrease_usage_counter(const request_handler_id& id) = 0;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_USAGE_INTERFACE_HPP
