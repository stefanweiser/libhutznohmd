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
    std::string asctime_date_string = "fRI Nov     6 08:49:37    1994";
    rest::http::http_date_parser parser(asctime_date_string);
    EXPECT_EQ(parser.buffer_, asctime_date_string);
    EXPECT_EQ(parser.index_, asctime_date_string.size());
    EXPECT_EQ(parser.error_, false);
    EXPECT_EQ(parser.timestamp_, 784111777);
    EXPECT_EQ(parser.scan_data_.parser, &parser);
}
