/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <http/parser/utility/push_back_string.hpp>

using namespace testing;

namespace rest
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
    EXPECT_EQ(s.size(), 4);
    EXPECT_EQ(std::string(s.c_str()), "0123");

    s.push_back('4');
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 5);
    EXPECT_EQ(std::string(s.c_str()), "01234");

    s.push_back('5');
    s.push_back('6');
    s.push_back('7');
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 8);
    EXPECT_EQ(std::string(s.c_str()), "01234567");

    s.push_back('8');
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 9);
    EXPECT_EQ(std::string(s.c_str()), "012345678");
    for (size_t i = 0; i < s.size(); i++) {
        EXPECT_EQ(s[i], '0' + i);
    }

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);

    s.append_string("abc");
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 3);
    EXPECT_EQ(std::string(s.c_str()), "abc");

    s.append_string("de");
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 5);
    EXPECT_EQ(std::string(s.c_str()), "abcde");
    for (size_t i = 0; i < s.size(); i++) {
        EXPECT_EQ(s[i], 'a' + i);
    }
}

} // namespace http

} // namespace rest
