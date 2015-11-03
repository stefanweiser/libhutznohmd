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

#ifndef LIBHUTZNOHMD_DEMUX_RESET_ERROR_HANDLER_INTERFACE_HPP
#define LIBHUTZNOHMD_DEMUX_RESET_ERROR_HANDLER_INTERFACE_HPP

#include "libhutznohmd/demux.hpp"

namespace hutzn
{

class reset_error_handler_interface
{
public:
    virtual ~reset_error_handler_interface(void);

    virtual void reset_error_handler(const http_status_code& code) = 0;
    virtual void disable(const http_status_code& code) = 0;
    virtual void enable(const http_status_code& code) = 0;
    virtual bool is_enabled(const http_status_code& code) const = 0;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_RESET_ERROR_HANDLER_INTERFACE_HPP
