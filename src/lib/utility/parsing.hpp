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

//! @brief Skips whitespace from the begin of a size-based string.
//!
//! Skips whitespace characters by increasing the data pointer until the first
//! character is neither space, tab, linefeed nor carriage return. The size
//! parameter (which actual is the length of the string) is decreased by the
//! number of increase steps. Does nothing on an empty string or a NULL, because
//! in both cases the size must be zero.
//! @param[in,out] data Points to the string.
//! @param[in,out] size Remaining bytes till the string's end.
inline void skip_whitespace(const char_t*& data, size_t& size)
{
    static const select_char_map map =
        make_select_char_map(' ', '\t', '\n', '\r');

    // loop until the end of the string is reached or the current front
    // character is no whitespace
    while ((size > 0) && (map[static_cast<uint8_t>(*data)])) {
        data++;
        size--;
    }
}

//! @brief Skips one character from a size-based string.
//!
//! Skips one character by increasing the data pointer once. The size parameter
//! (which actually is the length of the string) is decreased one time. Does
//! nothing on an empty string or on a NULL, because in both cases the size must
//! be zero.
//! @param[in,out] data Points to the string.
//! @param[in,out] size Remaining bytes till the string's end.
inline void skip_one_character(const char_t*& data, size_t& size)
{
    // do only something, when the size is greater zero
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
//! number. Returns -1 in case of nothing has been read and -2 on an overflow.
//! @param[in,out] data Points to the string.
//! @param[in,out] size Remaining bytes till the string's end.
//! @return             The parsed unsigned integer or -1 in case of nothing
//!                     read or -2 in case of an overflow.
template <typename result_type>
inline result_type parse_unsigned_integer(const char_t*& data, size_t& size)
{
    static_assert(std::is_signed<result_type>::value,
                  "need signed type for error values");
    result_type result;

    static const result_type nothing_read_error = -1;
    static const result_type overflow_error = -2;

    // when there is data to read and the first character is a digit
    if ((size > 0) && ((*data) >= '0') && ((*data) <= '9')) {
        result = 0;
        char_t character = *data;

        // loop until there are characters available, there is no overflow and
        // the current character is a digit
        do {
            result_type old = result;

            // shift the new digit in on the right side
            result = (result * decimal_base) + (character - '0');

            // consume the character
            data++;
            size--;

            // check for overflow
            if (old > result) {
                result = nothing_read_error;
                break;
            }

            character = *data;
        } while ((size > 0) && (character >= '0') && (character <= '9'));
    } else {
        result = overflow_error;
    }
    return result;
}

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_PARSING_HPP
