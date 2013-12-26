/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

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

#define private public
#define protected public

#include <http/server.hpp>

TEST(Conversion, Method)
{
    EXPECT_EQ(rest::convertMethod("HEAD"),    rest::http::Method::HEAD);
    EXPECT_EQ(rest::convertMethod("GET"),     rest::http::Method::GET);
    EXPECT_EQ(rest::convertMethod("PUT"),     rest::http::Method::PUT);
    EXPECT_EQ(rest::convertMethod("POST"),    rest::http::Method::POST);
    EXPECT_EQ(rest::convertMethod("DELETE"),  rest::http::Method::DELETE);
    EXPECT_EQ(rest::convertMethod("TRACE"),   rest::http::Method::TRACE);
    EXPECT_EQ(rest::convertMethod("OPTIONS"), rest::http::Method::OPTIONS);
    EXPECT_EQ(rest::convertMethod("CONNECT"), rest::http::Method::CONNECT);
    EXPECT_EQ(rest::convertMethod("TEST"),    rest::http::Method::UNKNOWN);
}

TEST(Conversion, Version)
{
    EXPECT_EQ(rest::convertVersion("HTTP/1.0"), rest::http::Version::HTTP_1_0);
    EXPECT_EQ(rest::convertVersion("HTTP/1.1"), rest::http::Version::HTTP_1_1);
    EXPECT_EQ(rest::convertVersion("HTTP/2.0"), rest::http::Version::HTTP_UNKNOWN);
}


TEST(Conversion, StatusCode)
{
    EXPECT_EQ(rest::convertStatusCode(rest::http::StatusCode::Ok), 200);
    EXPECT_EQ(rest::convertStatusCode(rest::http::StatusCode::NotFound), 404);
    EXPECT_EQ(rest::convertStatusCode(rest::http::StatusCode::InternalServerError), 500);
}
