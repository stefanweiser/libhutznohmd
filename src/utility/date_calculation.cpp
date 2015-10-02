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

#include <utility/common.hpp>

#include "date_calculation.hpp"

namespace hutzn
{

time_t day_of_the_year(const time_t day, const time_t month, const time_t year)
{
    time_t result = day;
    if (month < 3) {
        result += (306 * month - 301) / 10;
    } else {
        result += (306 * month - 913) / 10;
        if ((year % 4) == 0) {
            result += 60;
        } else {
            result += 59;
        }
    }
    return result;
}

bool is_valid_day(const time_t day, const time_t month, const time_t year)
{
    if (day < 1) {
        return false;
    }

    if (month > 7) {
        if ((month % 2) == 0) {
            return day <= 31;
        } else {
            return day <= 30;
        }
    } else {
        if (month == 2) {
            if ((year % 4) == 0) {
                return day <= 29;
            } else {
                return day <= 28;
            }
        } else if ((month % 2) == 0) {
            return day <= 30;
        } else {
            return day <= 31;
        }
    }
}

bool is_valid_epoch_date(const time_t day, const time_t month,
                         const time_t year)
{
    if ((year < 1970) || (!check_range<time_t, 1, 12>(month)) ||
        (!is_valid_day(day, month, year))) {
        return false;
    }
    return true;
}

time_t seconds_since_epoch(const time_t second_of_day, const time_t day,
                           const time_t month, const time_t year)
{
    if (!is_valid_epoch_date(day, month, year)) {
        return -1;
    }

    const time_t second_of_year =
        second_of_day + ((day_of_the_year(day, month, year) - 1) * 86400);
    const time_t year_seconds_since_epoch =
        ((year - 1970) * 86400 * 365) + (((year - (1972 - 3)) / 4) * 86400);

    return year_seconds_since_epoch + second_of_year;
}

} // namespace hutzn
