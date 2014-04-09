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

TEST(data, string_to_header_type)
{
    EXPECT_EQ(header_string_to_enum("custom"), rest::http::header_type::CUSTOM);
    EXPECT_EQ(header_string_to_enum("accept"), rest::http::header_type::ACCEPT);
    EXPECT_EQ(header_string_to_enum("accept-charset"), rest::http::header_type::ACCEPT_CHARSET);
    EXPECT_EQ(header_string_to_enum("accept-encoding"), rest::http::header_type::ACCEPT_ENCODING);
    EXPECT_EQ(header_string_to_enum("accept-language"), rest::http::header_type::ACCEPT_LANGUAGE);
    EXPECT_EQ(header_string_to_enum("accept-ranges"), rest::http::header_type::ACCEPT_RANGES);
    EXPECT_EQ(header_string_to_enum("age"), rest::http::header_type::AGE);
    EXPECT_EQ(header_string_to_enum("allow"), rest::http::header_type::ALLOW);
    EXPECT_EQ(header_string_to_enum("authorization"), rest::http::header_type::AUTHORIZATION);
    EXPECT_EQ(header_string_to_enum("cache-control"), rest::http::header_type::CACHE_CONTROL);
    EXPECT_EQ(header_string_to_enum("connection"), rest::http::header_type::CONNECTION);
    EXPECT_EQ(header_string_to_enum("content-encoding"),
              rest::http::header_type::CONTENT_ENCODING);
    EXPECT_EQ(header_string_to_enum("content-language"),
              rest::http::header_type::CONTENT_LANGUAGE);
    EXPECT_EQ(header_string_to_enum("content-length"), rest::http::header_type::CONTENT_LENGTH);
    EXPECT_EQ(header_string_to_enum("content-location"),
              rest::http::header_type::CONTENT_LOCATION);
    EXPECT_EQ(header_string_to_enum("content-md5"), rest::http::header_type::CONTENT_MD5);
    EXPECT_EQ(header_string_to_enum("content-range"), rest::http::header_type::CONTENT_RANGE);
    EXPECT_EQ(header_string_to_enum("content-type"), rest::http::header_type::CONTENT_TYPE);
    EXPECT_EQ(header_string_to_enum("cookie"), rest::http::header_type::COOKIE);
    EXPECT_EQ(header_string_to_enum("date"), rest::http::header_type::DATE);
    EXPECT_EQ(header_string_to_enum("etag"), rest::http::header_type::ETAG);
    EXPECT_EQ(header_string_to_enum("expect"), rest::http::header_type::EXPECT);
    EXPECT_EQ(header_string_to_enum("expires"), rest::http::header_type::EXPIRES);
    EXPECT_EQ(header_string_to_enum("from"), rest::http::header_type::FROM);
    EXPECT_EQ(header_string_to_enum("host"), rest::http::header_type::HOST);
    EXPECT_EQ(header_string_to_enum("if-match"), rest::http::header_type::IF_MATCH);
    EXPECT_EQ(header_string_to_enum("if-modified-since"),
              rest::http::header_type::IF_MODIFIED_SINCE);
    EXPECT_EQ(header_string_to_enum("if-none-match"), rest::http::header_type::IF_NONE_MATCH);
    EXPECT_EQ(header_string_to_enum("if-range"), rest::http::header_type::IF_RANGE);
    EXPECT_EQ(header_string_to_enum("if-unmodified-since"),
              rest::http::header_type::IF_UNMODIFIED_SINCE);
    EXPECT_EQ(header_string_to_enum("last-modified"), rest::http::header_type::LAST_MODIFIED);
    EXPECT_EQ(header_string_to_enum("location"), rest::http::header_type::LOCATION);
    EXPECT_EQ(header_string_to_enum("max-forwards"), rest::http::header_type::MAX_FORWARDS);
    EXPECT_EQ(header_string_to_enum("pragma"), rest::http::header_type::PRAGMA);
    EXPECT_EQ(header_string_to_enum("proxy-authenticate"),
              rest::http::header_type::PROXY_AUTHENTICATE);
    EXPECT_EQ(header_string_to_enum("proxy-authorization"),
              rest::http::header_type::PROXY_AUTHORIZATION);
    EXPECT_EQ(header_string_to_enum("range"), rest::http::header_type::RANGE);
    EXPECT_EQ(header_string_to_enum("referer"), rest::http::header_type::REFERER);
    EXPECT_EQ(header_string_to_enum("retry-after"), rest::http::header_type::RETRY_AFTER);
    EXPECT_EQ(header_string_to_enum("server"), rest::http::header_type::SERVER);
    EXPECT_EQ(header_string_to_enum("user-agent"), rest::http::header_type::USER_AGENT);
    EXPECT_EQ(header_string_to_enum("vary"), rest::http::header_type::VARY);
    EXPECT_EQ(header_string_to_enum("www-authenticate"),
              rest::http::header_type::WWW_AUTHENTICATE);
}
