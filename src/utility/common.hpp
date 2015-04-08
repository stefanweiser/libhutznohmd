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

template <typename type, const type lower_bound, const type upper_bound>
bool check_range(const type& value)
{
    static_assert(lower_bound <= upper_bound,
                  "Lower bound must be less or equal to upper bound.");
    if ((value < lower_bound) || (value > upper_bound)) {
        return false;
    }
    return true;
}

//! Checks if a given url path is valid. The path must begin with a slash and
//! double slashs are not allowed. Returns true, if the the path is valid.
static bool is_valid_url_path(const std::string& path)
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
    }
    return true;
}

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_COMMON_HPP
