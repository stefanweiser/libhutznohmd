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
    EXPECT_EQ(header_key_to_header_type("custom"), rest::http::header_type::CUSTOM);
    EXPECT_EQ(header_key_to_header_type("accept"), rest::http::header_type::ACCEPT);
    EXPECT_EQ(header_key_to_header_type("accept-charset"), rest::http::header_type::ACCEPT_CHARSET);
    EXPECT_EQ(header_key_to_header_type("accept-encoding"), rest::http::header_type::ACCEPT_ENCODING);
    EXPECT_EQ(header_key_to_header_type("accept-language"), rest::http::header_type::ACCEPT_LANGUAGE);
    EXPECT_EQ(header_key_to_header_type("accept-ranges"), rest::http::header_type::ACCEPT_RANGES);
    EXPECT_EQ(header_key_to_header_type("age"), rest::http::header_type::AGE);
    EXPECT_EQ(header_key_to_header_type("allow"), rest::http::header_type::ALLOW);
    EXPECT_EQ(header_key_to_header_type("authorization"), rest::http::header_type::AUTHORIZATION);
    EXPECT_EQ(header_key_to_header_type("cache-control"), rest::http::header_type::CACHE_CONTROL);
    EXPECT_EQ(header_key_to_header_type("connection"), rest::http::header_type::CONNECTION);
    EXPECT_EQ(header_key_to_header_type("content-encoding"),
              rest::http::header_type::CONTENT_ENCODING);
    EXPECT_EQ(header_key_to_header_type("content-language"),
              rest::http::header_type::CONTENT_LANGUAGE);
    EXPECT_EQ(header_key_to_header_type("content-length"), rest::http::header_type::CONTENT_LENGTH);
    EXPECT_EQ(header_key_to_header_type("content-location"),
              rest::http::header_type::CONTENT_LOCATION);
    EXPECT_EQ(header_key_to_header_type("content-md5"), rest::http::header_type::CONTENT_MD5);
    EXPECT_EQ(header_key_to_header_type("content-range"), rest::http::header_type::CONTENT_RANGE);
    EXPECT_EQ(header_key_to_header_type("content-type"), rest::http::header_type::CONTENT_TYPE);
    EXPECT_EQ(header_key_to_header_type("cookie"), rest::http::header_type::COOKIE);
    EXPECT_EQ(header_key_to_header_type("date"), rest::http::header_type::DATE);
    EXPECT_EQ(header_key_to_header_type("etag"), rest::http::header_type::ETAG);
    EXPECT_EQ(header_key_to_header_type("expect"), rest::http::header_type::EXPECT);
    EXPECT_EQ(header_key_to_header_type("expires"), rest::http::header_type::EXPIRES);
    EXPECT_EQ(header_key_to_header_type("from"), rest::http::header_type::FROM);
    EXPECT_EQ(header_key_to_header_type("host"), rest::http::header_type::HOST);
    EXPECT_EQ(header_key_to_header_type("if-match"), rest::http::header_type::IF_MATCH);
    EXPECT_EQ(header_key_to_header_type("if-modified-since"),
              rest::http::header_type::IF_MODIFIED_SINCE);
    EXPECT_EQ(header_key_to_header_type("if-none-match"), rest::http::header_type::IF_NONE_MATCH);
    EXPECT_EQ(header_key_to_header_type("if-range"), rest::http::header_type::IF_RANGE);
    EXPECT_EQ(header_key_to_header_type("if-unmodified-since"),
              rest::http::header_type::IF_UNMODIFIED_SINCE);
    EXPECT_EQ(header_key_to_header_type("last-modified"), rest::http::header_type::LAST_MODIFIED);
    EXPECT_EQ(header_key_to_header_type("location"), rest::http::header_type::LOCATION);
    EXPECT_EQ(header_key_to_header_type("max-forwards"), rest::http::header_type::MAX_FORWARDS);
    EXPECT_EQ(header_key_to_header_type("pragma"), rest::http::header_type::PRAGMA);
    EXPECT_EQ(header_key_to_header_type("proxy-authenticate"),
              rest::http::header_type::PROXY_AUTHENTICATE);
    EXPECT_EQ(header_key_to_header_type("proxy-authorization"),
              rest::http::header_type::PROXY_AUTHORIZATION);
    EXPECT_EQ(header_key_to_header_type("range"), rest::http::header_type::RANGE);
    EXPECT_EQ(header_key_to_header_type("referer"), rest::http::header_type::REFERER);
    EXPECT_EQ(header_key_to_header_type("retry-after"), rest::http::header_type::RETRY_AFTER);
    EXPECT_EQ(header_key_to_header_type("server"), rest::http::header_type::SERVER);
    EXPECT_EQ(header_key_to_header_type("user-agent"), rest::http::header_type::USER_AGENT);
    EXPECT_EQ(header_key_to_header_type("vary"), rest::http::header_type::VARY);
    EXPECT_EQ(header_key_to_header_type("www-authenticate"),
              rest::http::header_type::WWW_AUTHENTICATE);
}
