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

#ifndef LIBHUTZNOHMD_UTILITY_COMMON_HPP
#define LIBHUTZNOHMD_UTILITY_COMMON_HPP

#include <string>

namespace hutzn
{

//! Number of bits per byte.
static const size_t BITS_PER_BYTE = 8;

//! Number of bits per nibble.
static const size_t NIBBLE_SIZE = 4;

//! Contains the number of values, that can be represented by one byte.
static const size_t NUMBER_OF_VALUES_PER_BYTE =
    1 << (BITS_PER_BYTE * sizeof(uint8_t));

//! @brief Checking, whether a value is inbetween a range.
//!
//! Returns true if the value is in the interval [lower_bound, upper_bound].
template <typename type, const type lower_bound, const type upper_bound>
bool check_range(const type& value)
{
    static_assert(lower_bound <= upper_bound,
                  "Lower bound must be less or equal to upper bound.");
    return (value >= lower_bound) && (value <= upper_bound);
}

//! @brief Checks if a given url path is valid.
//!
//! The path must begin with a slash and double slashs are not allowed. Returns
//! true, if the the path is valid.
bool is_valid_uri_path(const std::string& path);

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_COMMON_HPP
