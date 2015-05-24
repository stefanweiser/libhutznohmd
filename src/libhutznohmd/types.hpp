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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_TYPES_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_TYPES_HPP

#include <cstdint>

namespace hutzn
{

//! Character typedef used in strings. Is as an exception to MISRA C++:2008 Rule
//! 3-9-2 defined without signed or unsignedness.
using char_t = char;

//! Unsigned 8-bit integer type.
using uint8_t = std::uint8_t;

//! Unsigned 16-bit integer type.
using uintl6_t = std::uint16_t;

//! Unsigned 32-bit integer type.
using uint32_t = std::uint32_t;

//! Unsigned 64-bit integer type.
using uint64_t = std::uint64_t;

//! Signed 8-bit integer type.
using int8_t = std::int8_t;

//! Signed 16-bit integer type.
using int16_t = std::int16_t;

//! Signed 32-bit integer type.
using int32_t = std::int32_t;

//! Signed 64-bit integer type.
using int64_t = std::int64_t;

//! Declares a variable as unused. Needed by variables, those single purpose is
//! to be used in an assertion.
#define UNUSED(x) ((void)x)

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_TYPES_HPP
