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

#include "libhutznohmd/types.hpp"

namespace hutzn
{

//! @brief Calculates an epoch timestamp.
//!
//! Given the number of seconds elapsed at a day and the date of that day, the
//! number of seconds elapsed since epoch time (00:00:00 UTC on 1 January 1970)
//! will be returned. Returns -1, when at least one parameter is not valid.
//! @param[in] second_of_day The second at a day. Must be in range of
//!                          [0..86399].
//! @param[in] day           Day of the month. The correct range depends on the
//!                          month and year, but at least in [1..31].
//! @param[in] month         Month of the year. Must stay in the range of
//!                          [1..12].
//! @param[in] year          Year, which must be in the range of [1970..2099].
epoch_time_t seconds_since_epoch(const uint32_t second_of_day,
                                 const uint8_t day, const uint8_t month,
                                 const uint16_t year);

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_DATE_CALCULATION_HPP
