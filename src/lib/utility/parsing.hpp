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

#ifndef LIBHUTZNOHMD_UTILITY_PARSING_HPP
#define LIBHUTZNOHMD_UTILITY_PARSING_HPP

#include "libhutznohmd/types.hpp"
#include "utility/common.hpp"
#include "utility/select_char_map.hpp"

namespace hutzn
{

//! @brief Skips whitespace from a size-based string.
//!
//! Skips whitespace characters by increasing the data pointer until the first
//! character is neither space, tabs, linefeed nor carriage return. The
//! size parameter (which actual is the length of the string) is decreased by
//! the number of increases. Does nothing on an empty string or a NULL.
inline void skip_whitespace(const char_t*& data, size_t& size)
{
    static const select_char_map map =
        make_select_char_map(' ', '\t', '\n', '\r');

    // Loop until the end of the string is reached or the current front
    // character is no whitespace.
    while ((size > 0) && (map[static_cast<uint8_t>(*data)])) {
        data++;
        size--;
    }
}

//! @brief Skips one character from a size-based string.
//!
//! Skips one character by increasing the data pointer once. The size parameter
//! (which actual is the length of the string) is decreased one time. Does
//! nothing on an empty string or on a NULL.
inline void skip_one_character(const char_t*& data, size_t& size)
{
    // Do only something, when the size is greater zero.
    if (size > 0) {
        data++;
        size--;
    }
}

//! @brief Parses an unsigned integer at the front of a string.
//!
//! The string has to begin with digits. These digits are read until a character
//! is no digit or the end of the string is reached. The data parameter is
//! increased by the number of characters read and the size is decreased by that
//! number. Returns -1 in case of nothing has been read or overflow.
inline int32_t parse_unsigned_integer(const char_t*& data, size_t& size)
{
    int32_t result;

    // If there is data to read and the first character is a digit.
    if ((size > 0) && ((*data) >= '0') && ((*data) <= '9')) {
        result = 0;
        char_t character = *data;

        // Loop until there are characters available, there is no overflow and
        // the current character is a digit.
        do {
            int32_t old = result;

            // Shift the new digit in on the right side.
            result = (result * DECIMAL_BASE) + (character - '0');

            // Consume the character.
            data++;
            size--;

            // Check for overflow.
            if (old > result) {
                result = -1;
                break;
            }

            character = *data;
        } while ((size > 0) && (character >= '0') && (character <= '9'));
    } else {
        result = -1;
    }
    return result;
}

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_PARSING_HPP
