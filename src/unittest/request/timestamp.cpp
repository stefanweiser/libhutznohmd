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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "request/timestamp.hpp"

using namespace testing;

namespace hutzn
{

TEST(timestamp, rfc1123_date)
{
    const std::string str = "Sun, 06 Nov 1994 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(784111777, timestamp);
}

TEST(timestamp, rfc1123_tolerant_date)
{
    const std::string str = "Sun,06Nov1994   08:49:37GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(784111777, timestamp);
}

TEST(timestamp, rfc1123_erroneous_month)
{
    const std::string str = "Sun, 06 Non 1994 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc1123_erroneous_hour)
{
    const std::string str = "Sun, 06 Nov 1994 24:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc1123_erroneous_minute)
{
    const std::string str = "Sun, 06 Nov 1994 08:60:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc1123_erroneous_second)
{
    const std::string str = "Sun, 06 Nov 1994 08:49:60 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc1123_erroneous_gmt)
{
    const std::string str = "Sun, 06 Nov 1994 08:49:37 TNT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_sunday)
{
    const std::string str = "Sunday, 14-Apr-85 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(482284800, timestamp);
}

TEST(timestamp, rfc850_tuesday)
{
    const std::string str = "Tuesday, 14-Apr-87 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(545356800, timestamp);
}

TEST(timestamp, rfc850_wednesday)
{
    const std::string str = "Wednesday, 14-Apr-93 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(734745600, timestamp);
}

TEST(timestamp, rfc850_thursday)
{
    const std::string str = "Thursday, 14-Apr-88 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(576979200, timestamp);
}

TEST(timestamp, rfc850_saturday)
{
    const std::string str = "Saturday, 14-Apr-90 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(640051200, timestamp);
}

TEST(timestamp, rfc850_tolerant_date)
{
    const std::string str = "Friday  ,06-Nov-94  08:49:37GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(784111777, timestamp);
}

TEST(timestamp, rfc850_erroneous_sunday)
{
    const std::string str = "Sunnyday,06 -  Nov   -   94  08:49:37GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_tuesday)
{
    const std::string str = "Tueday,06 -  Nov   -   94  08:49:37GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_wednesday)
{
    const std::string str = "Weddingday,06 -  Nov   -   94  08:49:37GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_thursday)
{
    const std::string str = "Thusday,06 -  Nov   -   94  08:49:37GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_saturday)
{
    const std::string str = "Saturnday,06 -  Nov   -   94  08:49:37GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_noday)
{
    const std::string str = "Noday, 06-Nov-94 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_monday)
{
    const std::string str = "Monday 06-Nov-94 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_day)
{
    const std::string str = "Monday, 00-Nov-94 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_leap_day)
{
    const std::string str = "Monday, 30-Feb-96 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_first_hyphen)
{
    const std::string str = "Sunday, 06_Nov-94 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_second_hyphen)
{
    const std::string str = "Sunday, 06-Nov_94 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_year)
{
    const std::string str = "Sunday, 06-Nov-100 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, rfc850_erroneous_gmt)
{
    const std::string str = "Sunday, 06-Nov-94 08:49:37 TNT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, asctime_date)
{
    const std::string str = "Sun Nov  6 08:49:37 1994";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(784111777, timestamp);
}

TEST(timestamp, asctime_tolerant_date)
{
    const std::string str = "fRI Nov     6 08:49:37    1994";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(784111777, timestamp);
}

TEST(timestamp, asctime_erroneous_date)
{
    const std::string str = "Mon Jul 21 03:56:0 1969";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(-1, timestamp);
}

TEST(timestamp, jan_date)
{
    const std::string str = "Thu, 01 Jan 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(0, timestamp);
}

TEST(timestamp, feb_date)
{
    const std::string str = "Sun, 01 Feb 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(2678400, timestamp);
}

TEST(timestamp, mar_date)
{
    const std::string str = "Sun, 01 Mar 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(5097600, timestamp);
}

TEST(timestamp, apr_date)
{
    const std::string str = "Wed, 01 Apr 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(7776000, timestamp);
}

TEST(timestamp, may_date)
{
    const std::string str = "Fri, 01 May 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(10368000, timestamp);
}

TEST(timestamp, jun_date)
{
    const std::string str = "Mon, 01 Jun 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(13046400, timestamp);
}

TEST(timestamp, jul_date)
{
    const std::string str = "Wed, 01 Jul 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(15638400, timestamp);
}

TEST(timestamp, aug_date)
{
    const std::string str = "Sat, 01 Aug 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(18316800, timestamp);
}

TEST(timestamp, sep_date)
{
    const std::string str = "Tue, 01 Sep 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(20995200, timestamp);
}

TEST(timestamp, oct_date)
{
    const std::string str = "Thu, 01 Oct 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(23587200, timestamp);
}

TEST(timestamp, nov_date)
{
    const std::string str = "Sun, 01 Nov 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(26265600, timestamp);
}

TEST(timestamp, dec_date)
{
    const std::string str = "Thu, 01 Dec 1970 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(28857600, timestamp);
}

TEST(timestamp, year2000_date)
{
    const std::string str = "Wed, 01 Mar 2000 00:00:00 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(951868800, timestamp);
}

TEST(timestamp, space_before_date)
{
    const std::string str = " Sun, 06 Nov 1994 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(784111777, timestamp);
}

TEST(timestamp, tab_before_date)
{
    const std::string str = "\tSun, 06 Nov 1994 08:49:37 GMT";
    const time_t timestamp = parse_timestamp(str.data(), str.size());
    EXPECT_EQ(784111777, timestamp);
}

} // namespace hutzn
