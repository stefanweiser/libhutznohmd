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

#include <http/parser/http.hpp>

using namespace testing;

int get_char(void * handle)
{
    return static_cast<std::istream *>(handle)->get();
}

int peek_char(void * handle)
{
    return static_cast<std::istream *>(handle)->peek();
}

class fixture
{
public:
    explicit fixture(const std::string & str)
        : httpscan_({anonymous_int_function(&get_char, &str_),
        anonymous_int_function(&peek_char, &str_), 0, lexer_state::UNFINISHED,
        push_back_string<40>(), push_back_string<1000>(), rest::http::method::UNKNOWN,
        rest::http::version::HTTP_UNKNOWN, push_back_string<1000>(), 0, push_back_string<100>(),
        std::map<std::string, std::string>(), 0, time(NULL)
    }),
    str_(str) {
    }

    time_t parse() {
        int result = httpscan_.get_functor_();
        return parse_timestamp(result, &httpscan_);
    }

    httpscan_t httpscan_;
    std::stringstream str_;
};

TEST(http_date_parser, rfc1123_date)
{
    fixture f("Sun, 06 Nov 1994 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date_parser, rfc1123_tolerant_date)
{
    fixture f("Sun,06Nov1994   08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date_parser, rfc850_date)
{
    fixture f("Sunday, 06-Nov-94 08:49:37 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date_parser, rfc850_tolerant_date)
{
    fixture f("Friday  ,06-Nov-94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date_parser, rfc850_erroneous_date)
{
    fixture f("Sunnyday,06 -  Nov   -   94  08:49:37GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, -1);
}

TEST(http_date_parser, asctime_date)
{
    fixture f("Sun Nov  6 08:49:37 1994");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date_parser, asctime_tolerant_date)
{
    fixture f("fRI Nov     6 08:49:37    1994");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 784111777);
}

TEST(http_date_parser, jan_date)
{
    fixture f("Thu, 01 Jan 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 0);
}

TEST(http_date_parser, feb_date)
{
    fixture f("Sun, 01 Feb 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 2678400);
}

TEST(http_date_parser, mar_date)
{
    fixture f("Sun, 01 Mar 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 5097600);
}

TEST(http_date_parser, apr_date)
{
    fixture f("Wed, 01 Apr 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 7776000);
}

TEST(http_date_parser, may_date)
{
    fixture f("Fri, 01 May 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 10368000);
}

TEST(http_date_parser, jun_date)
{
    fixture f("Mon, 01 Jun 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 13046400);
}

TEST(http_date_parser, jul_date)
{
    fixture f("Wed, 01 Jul 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 15638400);
}

TEST(http_date_parser, aug_date)
{
    fixture f("Sat, 01 Aug 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 18316800);
}

TEST(http_date_parser, sep_date)
{
    fixture f("Tue, 01 Sep 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 20995200);
}

TEST(http_date_parser, oct_date)
{
    fixture f("Thu, 01 Oct 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 23587200);
}

TEST(http_date_parser, nov_date)
{
    fixture f("Sun, 01 Nov 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 26265600);
}

TEST(http_date_parser, dec_date)
{
    fixture f("Thu, 01 Dec 1970 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 28857600);
}

TEST(http_date_parser, year2000_date)
{
    fixture f("Wed, 01 Mar 2000 00:00:00 GMT");
    const time_t timestamp = f.parse();
    EXPECT_EQ(timestamp, 951868800);
}

