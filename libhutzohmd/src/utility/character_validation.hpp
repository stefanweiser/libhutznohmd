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

#ifndef LIBHUTZNOHMD_UTILITY_CHARACTER_VALIDATION_HPP
#define LIBHUTZNOHMD_UTILITY_CHARACTER_VALIDATION_HPP

#include <array>

#include "libhutznohmd/types.hpp"
#include "utility/common.hpp"

namespace hutzn
{

//! Use binary-or-operation with this value to convert characters into lower
//! case.
static const uint8_t letter_case_offset = 0x20U;

//! @brief Converts a letter into the corresponding lower case letter.
//!
//! Simply returns the character, when the character is no upper case letter.
//! @param[in] c Character to convert.
//! @return      The lower case variant of c.
inline char_t to_lower(const char_t c)
{
    char_t result;
    if (check_range<char_t, 'A', 'Z'>(c)) {
        result = (c | letter_case_offset);
    } else {
        result = c;
    }
    return result;
}

//! @brief Converts a character with hexadecimal encoding to its digit.
//!
//! Returns 255, when the input character is not hexadecimal or the integral
//! representation of the character (e.g. 5 for '5' and 11 for 'b'). Conversion
//! works case insensitive (e.g. 'c' represents the same as 'C').
//! @param[in] c Character to convert.
//! @return      -1 or the integer representation of c.
inline uint8_t from_hex(const char_t c)
{
    static const std::array<uint8_t, byte_state_count> hex_conversion_map = {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
         0xFF, 0xFF, 0xFF, 0xFF}};

    static_assert(sizeof(char_t) == sizeof(uint8_t),
                  "sizes of char_t and uint8_t are not equal");
    return hex_conversion_map[static_cast<uint8_t>(c)];
}

//! @brief Returns true, if the given character is a valid character for a URL
//! path.
//!
//! Valid characters are described in RFC 3986 at chapter 3.3 as token "pchar".
//! @param[in] c Character to check for validity.
//! @return      True if c is valid and false when not.
inline bool is_valid_url_path_character(char_t c)
{
    static const std::array<bool, byte_state_count> uri_path_validity_map = {
        {false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, true,  false, false, true,  true,  true,  true,
         true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
         true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
         false, true,  false, false, true,  true,  true,  true,  true,  true,
         true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
         true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
         true,  true,  false, true,  false, true,  false, true,  true,  true,
         true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
         true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
         true,  true,  true,  false, false, false, true,  false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false, false, false, false, false,
         false, false, false, false, false, false}};

    static_assert(sizeof(char_t) == sizeof(uint8_t),
                  "sizes of char_t and uint8_t are not equal");
    return uri_path_validity_map[static_cast<uint8_t>(c)];
}

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_CHARACTER_VALIDATION_HPP
