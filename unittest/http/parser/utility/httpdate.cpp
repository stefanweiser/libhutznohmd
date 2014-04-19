/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/parser/utility/httpdate.hpp>

using namespace testing;

namespace rest
{

namespace http
{

namespace
{

typedef std::pair<std::string, size_t> string_index_pair;

int32_t get_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return p->first[p->second++];
    }
    return -1;
}

int32_t peek_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return p->first[p->second];
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string & str);
    time_t parse();

    std::string str_;
};

fixture::fixture(const std::string & str)
    : str_(str)
{}

time_t fixture::parse()
{
    string_index_pair p(str_, 0);
    lexer l(anonymous_int_function(&get_char, &p),
            anonymous_int_function(&peek_char, &p));
    int32_t result = l.get();
    return parse_timestamp(result, l);
}

} // namespace

TEST(http_date, rfc1123_date)
{
    fixture f("Sun, 06 Nov 1994 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date, rfc1123_tolerant_date)
{
    fixture f("Sun,06Nov1994   08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date, rfc1123_erroneous_month)
{
    fixture f("Sun, 06 Non 1994 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc1123_erroneous_hour)
{
    fixture f("Sun, 06 Nov 1994 24:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc1123_erroneous_minute)
{
    fixture f("Sun, 06 Nov 1994 08:60:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc1123_erroneous_second)
{
    fixture f("Sun, 06 Nov 1994 08:49:60 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc850_sunday)
{
    fixture f("Sunday, 14-Apr-85 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 482284800);
}

TEST(http_date, rfc850_tuesday)
{
    fixture f("Tuesday, 14-Apr-87 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 545356800);
}

TEST(http_date, rfc850_wednesday)
{
    fixture f("Wednesday, 14-Apr-93 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 734745600);
}

TEST(http_date, rfc850_thursday)
{
    fixture f("Thursday, 14-Apr-88 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 576979200);
}

TEST(http_date, rfc850_saturday)
{
    fixture f("Saturday, 14-Apr-90 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 640051200);
}

TEST(http_date, rfc850_tolerant_date)
{
    fixture f("Friday  ,06-Nov-94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date, rfc850_erroneous_sunday)
{
    fixture f("Sunnyday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc850_erroneous_tuesday)
{
    fixture f("Tueday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc850_erroneous_wednesday)
{
    fixture f("Weddingday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc850_erroneous_thursday)
{
    fixture f("Thusday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc850_erroneous_saturday)
{
    fixture f("Saturnday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc850_erroneous_noday)
{
    fixture f("Noday, 06-Nov-94 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, rfc850_erroneous_monday)
{
    fixture f("Monday 06-Nov-94 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, asctime_date)
{
    fixture f("Sun Nov  6 08:49:37 1994");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date, asctime_tolerant_date)
{
    fixture f("fRI Nov     6 08:49:37    1994 ");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date, asctime_erroneous_date)
{
    fixture f("Mon Jul 21 03:56:0 1969");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date, jan_date)
{
    fixture f("Thu, 01 Jan 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 0);
}

TEST(http_date, feb_date)
{
    fixture f("Sun, 01 Feb 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 2678400);
}

TEST(http_date, mar_date)
{
    fixture f("Sun, 01 Mar 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 5097600);
}

TEST(http_date, apr_date)
{
    fixture f("Wed, 01 Apr 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 7776000);
}

TEST(http_date, may_date)
{
    fixture f("Fri, 01 May 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 10368000);
}

TEST(http_date, jun_date)
{
    fixture f("Mon, 01 Jun 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 13046400);
}

TEST(http_date, jul_date)
{
    fixture f("Wed, 01 Jul 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 15638400);
}

TEST(http_date, aug_date)
{
    fixture f("Sat, 01 Aug 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 18316800);
}

TEST(http_date, sep_date)
{
    fixture f("Tue, 01 Sep 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 20995200);
}

TEST(http_date, oct_date)
{
    fixture f("Thu, 01 Oct 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 23587200);
}

TEST(http_date, nov_date)
{
    fixture f("Sun, 01 Nov 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 26265600);
}

TEST(http_date, dec_date)
{
    fixture f("Thu, 01 Dec 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 28857600);
}

TEST(http_date, year2000_date)
{
    fixture f("Wed, 01 Mar 2000 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 951868800);
}

} // namespace http

} // namespace rest
