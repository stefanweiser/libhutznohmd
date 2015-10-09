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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <http/parser/utility/timestamp.hpp>

using namespace testing;

namespace hutzn
{

namespace http
{

namespace
{

using string_index_pair = std::pair<std::string, size_t>;

int32_t get_char(void* handle)
{
    string_index_pair* p = static_cast<string_index_pair*>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second++]);
    }
    return -1;
}

int32_t peek_char(void* handle)
{
    string_index_pair* p = static_cast<string_index_pair*>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second]);
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string& str);
    time_t parse();

    std::string str_;
};

fixture::fixture(const std::string& str)
    : str_(str)
{
}

time_t fixture::parse()
{
    string_index_pair p(str_, 0);
    lexer l(anonymous_int_function(&get_char, &p),
            anonymous_int_function(&peek_char, &p));
    int32_t result = l.get();
    return parse_timestamp(result, l);
}

} // namespace

TEST(old_timestamp, rfc1123_date)
{
    fixture f("Sun, 06 Nov 1994 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(784111777, timestamp);
}

TEST(old_timestamp, rfc1123_tolerant_date)
{
    fixture f("Sun,06Nov1994   08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(784111777, timestamp);
}

TEST(old_timestamp, rfc1123_erroneous_month)
{
    fixture f("Sun, 06 Non 1994 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc1123_erroneous_hour)
{
    fixture f("Sun, 06 Nov 1994 24:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc1123_erroneous_minute)
{
    fixture f("Sun, 06 Nov 1994 08:60:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc1123_erroneous_second)
{
    fixture f("Sun, 06 Nov 1994 08:49:60 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc1123_erroneous_gmt)
{
    fixture f("Sun, 06 Nov 1994 08:49:37 TNT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_sunday)
{
    fixture f("Sunday, 14-Apr-85 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(482284800, timestamp);
}

TEST(old_timestamp, rfc850_tuesday)
{
    fixture f("Tuesday, 14-Apr-87 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(545356800, timestamp);
}

TEST(old_timestamp, rfc850_wednesday)
{
    fixture f("Wednesday, 14-Apr-93 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(734745600, timestamp);
}

TEST(old_timestamp, rfc850_thursday)
{
    fixture f("Thursday, 14-Apr-88 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(576979200, timestamp);
}

TEST(old_timestamp, rfc850_saturday)
{
    fixture f("Saturday, 14-Apr-90 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(640051200, timestamp);
}

TEST(old_timestamp, rfc850_tolerant_date)
{
    fixture f("Friday  ,06-Nov-94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(784111777, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_sunday)
{
    fixture f("Sunnyday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_tuesday)
{
    fixture f("Tueday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_wednesday)
{
    fixture f("Weddingday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_thursday)
{
    fixture f("Thusday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_saturday)
{
    fixture f("Saturnday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_noday)
{
    fixture f("Noday, 06-Nov-94 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_monday)
{
    fixture f("Monday 06-Nov-94 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_day)
{
    fixture f("Monday, 00-Nov-94 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_leap_day)
{
    fixture f("Monday, 30-Feb-96 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_first_hyphen)
{
    fixture f("Sunday, 06_Nov-94 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_second_hyphen)
{
    fixture f("Sunday, 06-Nov_94 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_year)
{
    fixture f("Sunday, 06-Nov-100 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, rfc850_erroneous_gmt)
{
    fixture f("Sunday, 06-Nov-94 08:49:37 TNT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, asctime_date)
{
    fixture f("Sun Nov  6 08:49:37 1994");
    const time_t timestamp = f.parse();
    EXPECT_EQ(784111777, timestamp);
}

TEST(old_timestamp, asctime_tolerant_date)
{
    fixture f("fRI Nov     6 08:49:37    1994 ");
    const time_t timestamp = f.parse();
    EXPECT_EQ(784111777, timestamp);
}

TEST(old_timestamp, asctime_erroneous_date)
{
    fixture f("Mon Jul 21 03:56:0 1969");
    const time_t timestamp = f.parse();
    EXPECT_EQ(-1, timestamp);
}

TEST(old_timestamp, jan_date)
{
    fixture f("Thu, 01 Jan 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(0, timestamp);
}

TEST(old_timestamp, feb_date)
{
    fixture f("Sun, 01 Feb 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(2678400, timestamp);
}

TEST(old_timestamp, mar_date)
{
    fixture f("Sun, 01 Mar 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(5097600, timestamp);
}

TEST(old_timestamp, apr_date)
{
    fixture f("Wed, 01 Apr 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(7776000, timestamp);
}

TEST(old_timestamp, may_date)
{
    fixture f("Fri, 01 May 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(10368000, timestamp);
}

TEST(old_timestamp, jun_date)
{
    fixture f("Mon, 01 Jun 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(13046400, timestamp);
}

TEST(old_timestamp, jul_date)
{
    fixture f("Wed, 01 Jul 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(15638400, timestamp);
}

TEST(old_timestamp, aug_date)
{
    fixture f("Sat, 01 Aug 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(18316800, timestamp);
}

TEST(old_timestamp, sep_date)
{
    fixture f("Tue, 01 Sep 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(20995200, timestamp);
}

TEST(old_timestamp, oct_date)
{
    fixture f("Thu, 01 Oct 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(23587200, timestamp);
}

TEST(old_timestamp, nov_date)
{
    fixture f("Sun, 01 Nov 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(26265600, timestamp);
}

TEST(old_timestamp, dec_date)
{
    fixture f("Thu, 01 Dec 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(28857600, timestamp);
}

TEST(old_timestamp, year2000_date)
{
    fixture f("Wed, 01 Mar 2000 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(951868800, timestamp);
}

} // namespace http

} // namespace hutzn
