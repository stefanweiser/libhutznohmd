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

#include <http/parser/utility/push_back_string.hpp>

using namespace testing;

namespace hutzn
{

namespace http
{

TEST(push_back_string, basic_usage)
{
    push_back_string<4> s;

    EXPECT_TRUE(s.empty());

    s.push_back('0');
    s.push_back('1');
    s.push_back('2');
    s.push_back('3');
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(4, s.size());
    EXPECT_EQ("0123", std::string(s.c_str()));

    s.push_back('4');
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(5, s.size());
    EXPECT_EQ("01234", std::string(s.c_str()));

    s.push_back('5');
    s.push_back('6');
    s.push_back('7');
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(8, s.size());
    EXPECT_EQ("01234567", std::string(s.c_str()));

    s.push_back('8');
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(9, s.size());
    EXPECT_EQ("012345678", std::string(s.c_str()));
    for (size_t i = 0; i < s.size(); i++) {
        EXPECT_EQ('0' + i, s[i]);
    }

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(0, s.size());

    s.append_string("abc");
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(3, s.size());
    EXPECT_EQ("abc", std::string(s.c_str()));

    s.append_string("de");
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 5);
    EXPECT_EQ("abcde", std::string(s.c_str()));
    for (size_t i = 0; i < s.size(); i++) {
        EXPECT_EQ('a' + i, s[i]);
    }
}

} // namespace http

} // namespace hutzn
