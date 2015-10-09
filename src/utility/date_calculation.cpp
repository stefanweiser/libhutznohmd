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

namespace
{

static const time_t years_between_leapyears = 4;
static const time_t epoch_start_year = 1970;
static const time_t usual_days_per_year = 365;
static const time_t seconds_per_day = 86400;

time_t day_of_the_year(const time_t day, const time_t month, const time_t year)
{
    time_t result = day;
    static const int32_t base_days = 306;
    if (month < 3) {
        result += (base_days * month - 301) / 10;
    } else {
        result += (base_days * month - 913) / 10;
        static const int32_t normal_rest_days = 60;
        if ((year % years_between_leapyears) == 0) {
            result += normal_rest_days;
        } else {
            result += normal_rest_days - 1;
        }
    }
    return result;
}

bool is_valid_day(const time_t day, const time_t month, const time_t year)
{
    static const time_t long_month = 31;
    static const time_t normal_month = 30;
    static const time_t short_month = 28;
    static const time_t short_month_in_leapyear = 29;

    if (day < 1) {
        return false;
    }

    static const time_t juli = 7;
    static const time_t even_month_modulo = 2;
    if (month > juli) {
        if ((month % even_month_modulo) == 0) {
            return day <= long_month;
        } else {
            return day <= normal_month;
        }
    } else {
        static const time_t february = 2;
        if (month == february) {
            if ((year % years_between_leapyears) == 0) {
                return day <= short_month_in_leapyear;
            } else {
                return day <= short_month;
            }
        } else if ((month % even_month_modulo) == 0) {
            return day <= normal_month;
        } else {
            return day <= long_month;
        }
    }
}

bool is_valid_epoch_date(const time_t day, const time_t month,
                         const time_t year)
{
    static const time_t first_month = 1;
    static const time_t last_month = 12;

    if ((year < epoch_start_year) ||
        (!check_range<time_t, first_month, last_month>(month)) ||
        (!is_valid_day(day, month, year))) {
        return false;
    }
    return true;
}

} // namespace

epoch_time_t seconds_since_epoch(const uint32_t second_of_day,
                                 const uint8_t day, const uint8_t month,
                                 const uint32_t year)
{
    if (!is_valid_epoch_date(day, month, year)) {
        return -1;
    }

    const time_t second_of_year =
        second_of_day +
        ((day_of_the_year(day, month, year) - 1) * seconds_per_day);
    const time_t year_seconds_since_epoch =
        ((year - epoch_start_year) * seconds_per_day * usual_days_per_year) +
        (((year - (epoch_start_year - 1)) / years_between_leapyears) *
         seconds_per_day);

    return year_seconds_since_epoch + second_of_year;
}

} // namespace hutzn
