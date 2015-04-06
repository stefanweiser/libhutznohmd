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

#include <request/parser_data.hpp>

using namespace testing;

namespace hutzn
{

namespace request
{

TEST(parser_data, parse_mime_type_default)
{
    parser_data p;
    EXPECT_EQ(p.parse_mime_type("*", 1), mime_type::WILDCARD);
    EXPECT_EQ(p.parse_mime_type("application", 11), mime_type::APPLICATION);
    EXPECT_EQ(p.parse_mime_type("audio", 5), mime_type::AUDIO);
    EXPECT_EQ(p.parse_mime_type("image", 5), mime_type::IMAGE);
    EXPECT_EQ(p.parse_mime_type("text", 4), mime_type::TEXT);
    EXPECT_EQ(p.parse_mime_type("video", 5), mime_type::VIDEO);
}

TEST(parser_data, parse_mime_subtype_default)
{
    parser_data p;
    EXPECT_EQ(p.parse_mime_subtype("*", 1), mime_subtype::WILDCARD);
    EXPECT_EQ(p.parse_mime_subtype("plain", 5), mime_subtype::PLAIN);
}

} // namespace request

} // namespace hutzn
