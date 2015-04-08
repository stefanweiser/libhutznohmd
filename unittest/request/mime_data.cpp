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

#include <libhutznohmd/request.hpp>
#include <request/mime_data.hpp>

using namespace testing;

namespace hutzn
{

namespace request
{

TEST(mime_data, initialization)
{
    mime_data<mime_type, uint8_t> t;
    EXPECT_EQ(t.parse_type("abc", 3), mime_type::INVALID);
    EXPECT_EQ(t.parse_type("ABC", 3), mime_type::INVALID);
}

TEST(mime_data, registering_unregistering)
{
    mime_data<mime_type, uint8_t> t;
    mime_type type = t.register_type("abc");
    EXPECT_NE(type, mime_type::INVALID);
    EXPECT_TRUE(t.unregister_type(type));
}

TEST(mime_data, double_registering)
{
    mime_data<mime_type, uint8_t> t;
    mime_type type1 = t.register_type("abc");
    mime_type type2 = t.register_type("abc");
    EXPECT_NE(type1, mime_type::INVALID);
    EXPECT_EQ(type2, mime_type::INVALID);
    EXPECT_TRUE(t.unregister_type(type1));
    EXPECT_FALSE(t.unregister_type(type2));
}

TEST(mime_data, double_registering_case_sensitive)
{
    mime_data<mime_type, uint8_t> t;
    mime_type type1 = t.register_type("abc");
    mime_type type2 = t.register_type("ABC");
    EXPECT_NE(type1, mime_type::INVALID);
    EXPECT_EQ(type2, mime_type::INVALID);
    EXPECT_TRUE(t.unregister_type(type1));
    EXPECT_FALSE(t.unregister_type(type2));
}

TEST(mime_data, parse_type_successful)
{
    mime_data<mime_type, uint8_t> t;
    mime_type type = t.register_type("abc");
    EXPECT_NE(type, mime_type::INVALID);
    EXPECT_EQ(t.parse_type("abc", 3), type);
    EXPECT_EQ(t.parse_type("abcdef", 6), type);
    EXPECT_TRUE(t.unregister_type(type));
}

TEST(mime_data, parse_type_successful_case_sensitive)
{
    mime_data<mime_type, uint8_t> t;
    mime_type type = t.register_type("abc");
    EXPECT_NE(type, mime_type::INVALID);
    EXPECT_EQ(t.parse_type("Abc", 3), type);
    EXPECT_EQ(t.parse_type("aBc", 3), type);
    EXPECT_EQ(t.parse_type("abC", 3), type);
    EXPECT_EQ(t.parse_type("ABc", 3), type);
    EXPECT_EQ(t.parse_type("aBC", 3), type);
    EXPECT_EQ(t.parse_type("AbC", 3), type);
    EXPECT_EQ(t.parse_type("ABC", 3), type);
    EXPECT_TRUE(t.unregister_type(type));
}

TEST(mime_data, is_registered)
{
    mime_data<mime_type, uint8_t> t;
    mime_type type = t.register_type("abc");
    EXPECT_NE(type, mime_type::INVALID);
    EXPECT_TRUE(t.is_registered(type));
    EXPECT_TRUE(t.unregister_type(type));
    EXPECT_FALSE(t.is_registered(type));
}

TEST(mime_data, parse_type_failure)
{
    mime_data<mime_type, uint8_t> t;
    mime_type type = t.register_type("abc");
    EXPECT_NE(type, mime_type::INVALID);
    EXPECT_EQ(t.parse_type("xyzabcdef", 9), mime_type::INVALID);
    EXPECT_TRUE(t.unregister_type(type));
}

} // namespace request

} // namespace hutzn
