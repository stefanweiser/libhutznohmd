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

#include <http/parser/header/httpdateparser.h>

using namespace testing;

TEST(http_date_parser, empty_date)
{
    rest::http::http_date_parser parser("");
    EXPECT_EQ(parser.buffer_, "");
    EXPECT_EQ(parser.index_, 0);
    EXPECT_EQ(parser.error_, true);
    EXPECT_EQ(parser.timestamp_, -1);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, rfc1123_date)
{
    std::string rfc1123_date_string = "Sun, 06 Nov 1994 08:49:37 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.timestamp_, 784111777);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, rfc1123_tolerant_date)
{
    std::string rfc1123_date_string = "Sun  ,06Nov1994   08:49:37GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.timestamp_, 784111777);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, rfc850_date)
{
    std::string rfc850_date_string = "Sunday, 06-Nov-94 08:49:37 GMT";
    rest::http::http_date_parser parser(rfc850_date_string);
    EXPECT_EQ(parser.buffer_, rfc850_date_string);
    EXPECT_EQ(parser.index_, rfc850_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.timestamp_, 784111777);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, rfc850_tolerant_date)
{
    std::string rfc850_date_string = "Friday  ,06 -  Nov   -   94  08:49:37GMT";
    rest::http::http_date_parser parser(rfc850_date_string);
    EXPECT_EQ(parser.buffer_, rfc850_date_string);
    EXPECT_EQ(parser.index_, rfc850_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.timestamp_, 784111777);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, rfc850_erroneous_date)
{
    std::string rfc850_date_string = "Sunnyday,06 -  Nov   -   94  08:49:37GMT";
    rest::http::http_date_parser parser(rfc850_date_string);
    EXPECT_EQ(parser.buffer_, rfc850_date_string);
    EXPECT_EQ(parser.index_, 4);
    EXPECT_EQ(parser.error_, true);
    EXPECT_EQ(parser.timestamp_, -1);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, asctime_date)
{
    std::string asctime_date_string = "Sun Nov  6 08:49:37 1994";
    rest::http::http_date_parser parser(asctime_date_string);
    EXPECT_EQ(parser.buffer_, asctime_date_string);
    EXPECT_EQ(parser.index_, asctime_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.timestamp_, 784111777);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, asctime_tolerant_date)
{
    std::string asctime_date_string = "fRI\tNov     6 08:49:37    1994";
    rest::http::http_date_parser parser(asctime_date_string);
    EXPECT_EQ(parser.buffer_, asctime_date_string);
    EXPECT_EQ(parser.index_, asctime_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.timestamp_, 784111777);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, jan_date)
{
    std::string rfc1123_date_string = "Thu, 01 Jan 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 0);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, feb_date)
{
    std::string rfc1123_date_string = "Sun, 01 Feb 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 2678400);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, mar_date)
{
    std::string rfc1123_date_string = "Sun, 01 Mar 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 5097600);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, apr_date)
{
    std::string rfc1123_date_string = "Wed, 01 Apr 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 7776000);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, may_date)
{
    std::string rfc1123_date_string = "Fri, 01 May 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 10368000);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, jun_date)
{
    std::string rfc1123_date_string = "Mon, 01 Jun 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 13046400);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, jul_date)
{
    std::string rfc1123_date_string = "Wed, 01 Jul 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 15638400);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, aug_date)
{
    std::string rfc1123_date_string = "Sat, 01 Aug 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 18316800);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, sep_date)
{
    std::string rfc1123_date_string = "Tue, 01 Sep 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 20995200);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, oct_date)
{
    std::string rfc1123_date_string = "Thu, 01 Oct 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 23587200);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, nov_date)
{
    std::string rfc1123_date_string = "Sun, 01 Nov 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 26265600);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, dec_date)
{
    std::string rfc1123_date_string = "Thu, 01 Dec 1970 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 28857600);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

TEST(http_date_parser, year2000_date)
{
    std::string rfc1123_date_string = "Wed, 01 Mar 2000 00:00:00 GMT";
    rest::http::http_date_parser parser(rfc1123_date_string);
    EXPECT_EQ(parser.buffer_, rfc1123_date_string);
    EXPECT_EQ(parser.index_, rfc1123_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.valid(), true);
    EXPECT_EQ(parser.timestamp_, 951868800);
    EXPECT_EQ(parser.timestamp(), parser.timestamp_);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}

