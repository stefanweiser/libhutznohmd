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

#include <libhutznohmd/types.hpp>
#include <utility/select_char_map.hpp>

namespace hutzn
{

inline void skip_whitespace(const char_t*& data, size_t& remaining)
{
    static const select_char_map map = make_select_char_map(' ', '\n');
    while ((remaining > 0) && (true == map[static_cast<uint8_t>(*data)])) {
        data++;
        remaining--;
    }
}

inline void skip_one_character(const char_t*& data, size_t& remaining)
{
    if (remaining > 0) {
        data++;
        remaining--;
    }
}

inline int32_t parse_unsigned_integer(const char_t*& data, size_t& remaining)
{
    char_t character = *data;
    int32_t result = 0;
    if (remaining > 0) {
        while ((remaining > 0) && (character >= '0') && (character <= '9')) {
            int32_t old_result = result;
            result = (result * 10) + (character - 0x30);

            // Check for overflow.
            if (old_result > result) {
                result = -1;
                break;
            }

            data++;
            remaining--;
            character = *data;
        }
    } else {
        result = -1;
    }
    return result;
}

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_PARSING_HPP
