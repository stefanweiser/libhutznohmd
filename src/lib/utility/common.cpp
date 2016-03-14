/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#include "common.hpp"

#include "utility/character_validation.hpp"

namespace hutzn
{

bool is_valid_url_path(const std::string& path)
{
    const char_t slash = '/';
    bool must_be_a_slash = true;
    bool could_be_a_slash = true;

    // the loop over every character of the path tries to proove, that this is
    // not a valid path
    bool result = true;
    for (const char_t& c : path) {

        if (must_be_a_slash) {
            // the first character must be a slash
            if (c != slash) {
                result = false;
                break;
            }

            // never check this again
            must_be_a_slash = false;
        }

        // it could be a slash, when no slash is directly preceding it
        if (could_be_a_slash) {
            if (c == slash) {
                // the next character must not be a slash
                could_be_a_slash = false;
            }
        } else {
            if (c == slash) {
                // when the character is a slash even though is must not be one
                // this path is not valid
                result = false;
                break;
            }

            // the next one could be a slash again
            could_be_a_slash = true;
        }

        // in any case the character must be a valid one
        if (!is_valid_url_path_character(c)) {
            result = false;
            break;
        }
    }

    return result;
}

} // namespace hutzn
