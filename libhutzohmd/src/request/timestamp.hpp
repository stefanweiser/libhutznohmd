/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_REQUEST_TIMESTAMP_HPP
#define LIBHUTZNOHMD_REQUEST_TIMESTAMP_HPP

#include <ctime>

#include "request/lexer.hpp"

namespace hutzn
{

//! @brief Parses a RFC 850, RFC 1123 or ASCII-time timestamp from a string.
//!
//! The data pointer and the length will be modified. On success the data
//! pointer will point on the first character beyond the timestamp afterwards
//! and the length gets decreased by the number of bytes read.
//! @warning When the operation fails, both values are getting undefined.
//! @param[in,out] data   Data buffer pointing to the begin of the timestamp
//!                       representation.
//! @param[in,out] length Remaining number of bytes in the buffer.
//! @return               Timestamp in epoch time or -1 on error.
time_t parse_timestamp(const char_t* data, size_t length);

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_TIMESTAMP_HPP
