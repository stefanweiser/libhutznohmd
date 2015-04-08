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

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_COMMON_HPP
