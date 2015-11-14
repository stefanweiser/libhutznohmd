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

#include "date_calculation.hpp"

#include <array>

#include "utility/common.hpp"

namespace hutzn
{

namespace
{

static const uint8_t years_between_leapyears = 4;
static const uint8_t last_month = 12;
static const uint16_t epoch_start_year = 1970;

//! @brief Returns true, if a year is a leapyear.
//!
//! Does not check its boundaries, but is only valid within.
//! @param[in] year  Year [1970..2099]
//! @return          True if the given year is a leapyear and false when not.
bool is_leapyear(const uint16_t year)
{
    return (year % years_between_leapyears) == 0;
}

//! @brief Returns the 0-based day of the year.
//!
//! This means 0 for 1st January and 365 for 31st December of a leapyear. The
//! input parameters are not getting range checked.
//! @param[in] day   Day in the month [1..MAX_DAY_OF_MONTH]
//! @param[in] month Month of the year [1..12]
//! @param[in] year  Year [1970..2099]
//! @return          The number of days passed in this year.
uint16_t day_of_the_year(const uint8_t day, const uint8_t month,
                         const uint16_t year)
{
    static const std::array<uint16_t, last_month> days_in_year_per_month = {
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}};

    // is in range of [0..365]
    uint16_t result =
        static_cast<uint16_t>(days_in_year_per_month[month - 1] + (day - 1));
    if (is_leapyear(year)) {
        result++;
    }
    return result;
}

//! @brief Returns true, when the given date is valid.
//!
//! Runs some range checks on the parameters and returns true, if the values
//! passes all checks.
//! @param[in] day   Day in the month [1..MAX_DAY_OF_MONTH]
//! @param[in] month Month of the year [1..12]
//! @param[in] year  Year [1970..2099]
//! @return          True if the given date is valid and false when not.
bool is_valid_epoch_date(const uint8_t day, const uint8_t month,
                         const uint16_t year)
{
    // only century turns divisible by 400 are leapyears and therefore this
    // algorithm is only valid between 1970 and 2099
    static const uint8_t february = 2;
    static const uint8_t first_month = 1;
    static const uint16_t last_valid_year = 2099;

    // a date is valid, when all of the following conditions are met:
    // - the year is greater than or equal to the start year of epoch time
    // - the month must be between 1 and 12
    // - the day is between 1 and the correct days_per_month value, which
    //   depends on month and year
    bool result;
    if ((!check_range<uint16_t, epoch_start_year, last_valid_year>(year)) ||
        (!check_range<uint8_t, first_month, last_month>(month)) || (day < 1)) {
        result = false;
    } else {
        // note that the month is 1-based and the index of this array is 0-based
        static const std::array<uint8_t, last_month> days_per_month = {
            {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
        if ((month == february) && is_leapyear(year)) {
            result = day <= (days_per_month[month - 1] + 1);
        } else {
            result = day <= days_per_month[month - 1];
        }
    }
    return result;
}

} // namespace

epoch_time_t seconds_since_epoch(const uint32_t second_of_day,
                                 const uint8_t day, const uint8_t month,
                                 const uint16_t year)
{
    static const uint16_t usual_days_per_year = 365;
    static const uint32_t seconds_per_day = 86400;

    epoch_time_t result;
    if ((!is_valid_epoch_date(day, month, year)) ||
        (second_of_day >= seconds_per_day)) {
        // not valid
        result = -1;
    } else {
        // somewhere in [0..129]
        const uint16_t year_since_epoch =
            static_cast<uint16_t>(year - epoch_start_year);

        // since 1972 was the first leapyear in epoch, the leapyear calculation
        // has to start with 1969 or the result of 1973 would mistakenly be 0
        const uint16_t days_due_to_leapyears = static_cast<uint16_t>(
            (year_since_epoch + 1) / years_between_leapyears);

        // calculate days since epoch without days due to a leapyear, add the
        // days due to a leapyear and add the days passed in the current year
        const uint32_t days_since_epoch =
            (year_since_epoch * usual_days_per_year) + days_due_to_leapyears +
            day_of_the_year(day, month, year);

        result =
            (static_cast<epoch_time_t>(days_since_epoch) * seconds_per_day) +
            second_of_day;
    }
    return result;
}

} // namespace hutzn
