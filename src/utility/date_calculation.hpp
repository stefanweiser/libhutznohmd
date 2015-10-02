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

#ifndef LIBHUTZNOHMD_UTILITY_DATE_CALCULATION_HPP
#define LIBHUTZNOHMD_UTILITY_DATE_CALCULATION_HPP

#include <ctime>

namespace hutzn
{

time_t seconds_since_epoch(const time_t second_of_day, const time_t day,
                           const time_t month, const time_t year);

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_DATE_CALCULATION_HPP
