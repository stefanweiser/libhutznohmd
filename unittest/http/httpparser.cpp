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
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/parser/lexer.h>

using namespace testing;

TEST(HttpParser, ConstructionDestruction)
{
    rest::http::HttpParser parser;
    EXPECT_EQ(parser.m_finished, false);
    EXPECT_EQ(parser.m_lastChar, 0);
    EXPECT_EQ(parser.m_headerKey.empty(), true);
    EXPECT_EQ(parser.m_headerValue.empty(), true);
    EXPECT_EQ(parser.m_method, METHOD_UNKNOWN);
    EXPECT_EQ(parser.m_method, VERSION_UNKNOWN);
    EXPECT_EQ(parser.m_uri.empty(), true);
    EXPECT_EQ(parser.m_statusCode, 0);
    EXPECT_EQ(parser.m_reasonPhrase.empty(), true);
    EXPECT_EQ(parser.m_headers.empty(), true);
}
