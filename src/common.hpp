/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREST_COMMON_HPP
#define LIBREST_COMMON_HPP

//! This define can be used to declare a symbol as unused.
#define UNUSED __attribute__((unused))

namespace rest
{

template <typename type, type lower_bound, type upper_bound>
bool check_range(const type& value)
{
    static_assert(lower_bound <= upper_bound,
                  "Lower bound must be less or equal to upper bound.");
    if ((value < lower_bound) || (value > upper_bound)) {
        return false;
    }
    return true;
}

} // namespace rest

#endif // LIBREST_COMMON_HPP
