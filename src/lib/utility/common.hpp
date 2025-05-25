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

#ifndef LIBHUTZNOHMD_UTILITY_COMMON_HPP
#define LIBHUTZNOHMD_UTILITY_COMMON_HPP

#include <cstdint>
#include <string>

namespace hutzn
{

//! Base of the decimal system.
static const std::uint8_t decimal_base = 10;

//! Number of bits per byte.
static const std::size_t bits_per_byte = 8;

//! Number of bits per nibble.
static const std::size_t nibble_size = 4;

//! Contains the number of values, that can be represented by one byte.
static const std::size_t byte_state_count =
    1 << (bits_per_byte * sizeof(uint8_t));

//! @brief Checking, whether a value is inbetween a range.
//!
//! Returns true if the value is in the interval [lower_bound, upper_bound].
//! @param[in] value Gets checked against the functions boundaries.
//! @return          True if value is in range and false when not.
template <typename type, const type lower_bound, const type upper_bound>
bool check_range(const type& value)
{
    static_assert(lower_bound <= upper_bound,
                  "lower bound must be less or equal to upper bound");
    return (value >= lower_bound) && (value <= upper_bound);
}

//! @brief Checks if a given url path is valid.
//!
//! Note that this method implements just a subset of RFC 3986 chapter 3.3.
//! @see request_handler_id for more details. Returns true, if the the path is
//! valid.
//! @param[in] path The path that gets checked for validity.
//! @return         True if path is a valid url path and false when not.
bool is_valid_url_path(const std::string& path);

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_COMMON_HPP
