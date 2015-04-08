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

#include <utility/character_validation.hpp>

#include "common.hpp"

namespace hutzn
{

bool is_valid_uri_path(const std::string& path)
{
    const char slash = '/';
    bool must_be_a_slash = true;
    bool could_be_a_slash = true;

    for (const char& c : path) {
        if (true == must_be_a_slash) {
            if (c != slash) {
                return false;
            }
            must_be_a_slash = false;
        }

        if (true == could_be_a_slash) {
            if (c == slash) {
                could_be_a_slash = false;
            } else {
                could_be_a_slash = true;
            }
        } else {
            if (c == slash) {
                return false;
            }
            could_be_a_slash = true;
        }

        if (false == is_valid_uri_path_character(static_cast<uint8_t>(c))) {
            return false;
        }
    }
    return true;
}

} // namespace hutzn
