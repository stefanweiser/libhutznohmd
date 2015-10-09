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

#include "error_handler.hpp"

namespace hutzn
{

error_handler::error_handler(reset_error_handler_interface& request_processor,
                             const http_status_code& code)
    : request_processor_(request_processor)
    , code_(code)
{
}

error_handler::~error_handler(void) noexcept(true)
{
    request_processor_.reset_error_handler(code_);
}

void error_handler::disable(void)
{
    request_processor_.disable(code_);
}

void error_handler::enable(void)
{
    request_processor_.enable(code_);
}

bool error_handler::is_enabled(void) const
{
    return request_processor_.is_enabled(code_);
}

} // namespace hutzn
