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

#include <http/parser/data.h>

using namespace testing;

TEST(Data, StringToHeaderType)
{
    EXPECT_EQ(rest::http::headerStringToEnum("custom"), rest::http::HeaderType::Custom);
    EXPECT_EQ(rest::http::headerStringToEnum("accept"), rest::http::HeaderType::Accept);
    EXPECT_EQ(rest::http::headerStringToEnum("accept-charset"), rest::http::HeaderType::AcceptCharset);
    EXPECT_EQ(rest::http::headerStringToEnum("accept-encoding"),
              rest::http::HeaderType::AcceptEncoding);
    EXPECT_EQ(rest::http::headerStringToEnum("accept-language"),
              rest::http::HeaderType::AcceptLanguage);
    EXPECT_EQ(rest::http::headerStringToEnum("accept-ranges"), rest::http::HeaderType::AcceptRanges);
    EXPECT_EQ(rest::http::headerStringToEnum("age"), rest::http::HeaderType::Age);
    EXPECT_EQ(rest::http::headerStringToEnum("allow"), rest::http::HeaderType::Allow);
    EXPECT_EQ(rest::http::headerStringToEnum("authorization"), rest::http::HeaderType::Authorization);
    EXPECT_EQ(rest::http::headerStringToEnum("cache-control"), rest::http::HeaderType::CacheControl);
    EXPECT_EQ(rest::http::headerStringToEnum("connection"), rest::http::HeaderType::Connection);
    EXPECT_EQ(rest::http::headerStringToEnum("content-encoding"),
              rest::http::HeaderType::ContentEncoding);
    EXPECT_EQ(rest::http::headerStringToEnum("content-language"),
              rest::http::HeaderType::ContentLanguage);
    EXPECT_EQ(rest::http::headerStringToEnum("content-length"), rest::http::HeaderType::ContentLength);
    EXPECT_EQ(rest::http::headerStringToEnum("content-location"),
              rest::http::HeaderType::ContentLocation);
    EXPECT_EQ(rest::http::headerStringToEnum("content-md5"), rest::http::HeaderType::ContentMD5);
    EXPECT_EQ(rest::http::headerStringToEnum("content-range"), rest::http::HeaderType::ContentRange);
    EXPECT_EQ(rest::http::headerStringToEnum("content-type"), rest::http::HeaderType::ContentType);
    EXPECT_EQ(rest::http::headerStringToEnum("cookie"), rest::http::HeaderType::Cookie);
    EXPECT_EQ(rest::http::headerStringToEnum("date"), rest::http::HeaderType::Date);
    EXPECT_EQ(rest::http::headerStringToEnum("etag"), rest::http::HeaderType::ETag);
    EXPECT_EQ(rest::http::headerStringToEnum("expect"), rest::http::HeaderType::Expect);
    EXPECT_EQ(rest::http::headerStringToEnum("expires"), rest::http::HeaderType::Expires);
    EXPECT_EQ(rest::http::headerStringToEnum("from"), rest::http::HeaderType::From);
    EXPECT_EQ(rest::http::headerStringToEnum("host"), rest::http::HeaderType::Host);
    EXPECT_EQ(rest::http::headerStringToEnum("if-match"), rest::http::HeaderType::IfMatch);
    EXPECT_EQ(rest::http::headerStringToEnum("if-modified-since"),
              rest::http::HeaderType::IfModifiedSince);
    EXPECT_EQ(rest::http::headerStringToEnum("if-none-match"), rest::http::HeaderType::IfNoneMatch);
    EXPECT_EQ(rest::http::headerStringToEnum("if-range"), rest::http::HeaderType::IfRange);
    EXPECT_EQ(rest::http::headerStringToEnum("if-unmodified-since"),
              rest::http::HeaderType::IfUnmodifiedSince);
    EXPECT_EQ(rest::http::headerStringToEnum("last-modified"), rest::http::HeaderType::LastModified);
    EXPECT_EQ(rest::http::headerStringToEnum("location"), rest::http::HeaderType::Location);
    EXPECT_EQ(rest::http::headerStringToEnum("max-forwards"), rest::http::HeaderType::MaxForwards);
    EXPECT_EQ(rest::http::headerStringToEnum("proxy-authenticate"),
              rest::http::HeaderType::ProxyAuthenticate);
    EXPECT_EQ(rest::http::headerStringToEnum("proxy-authorization"),
              rest::http::HeaderType::ProxyAuthorization);
    EXPECT_EQ(rest::http::headerStringToEnum("range"), rest::http::HeaderType::Range);
    EXPECT_EQ(rest::http::headerStringToEnum("referer"), rest::http::HeaderType::Referer);
    EXPECT_EQ(rest::http::headerStringToEnum("retry-after"), rest::http::HeaderType::RetryAfter);
    EXPECT_EQ(rest::http::headerStringToEnum("server"), rest::http::HeaderType::Server);
    EXPECT_EQ(rest::http::headerStringToEnum("te"), rest::http::HeaderType::TE);
    EXPECT_EQ(rest::http::headerStringToEnum("user-agent"), rest::http::HeaderType::UserAgent);
    EXPECT_EQ(rest::http::headerStringToEnum("vary"), rest::http::HeaderType::Vary);
    EXPECT_EQ(rest::http::headerStringToEnum("www-authenticate"),
              rest::http::HeaderType::WWWAuthenticate);
}
