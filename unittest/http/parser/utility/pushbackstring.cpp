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

#include <http/parser/utility/pushbackstring.hpp>

using namespace testing;

namespace rest
{

namespace http
{

TEST(push_back_string, basic_usage)
{
    push_back_string<4> s;
    EXPECT_EQ(s.empty(), true);
    s.push_back('0');
    s.push_back('1');
    s.push_back('2');
    s.push_back('3');
    EXPECT_EQ(s.dynamic_buffer_, nullptr);
    EXPECT_EQ(s.dynamic_size_, 0);
    EXPECT_EQ(std::string(s.c_str()), "0123");
    s.push_back('4');
    EXPECT_NE(s.dynamic_buffer_, nullptr);
    EXPECT_EQ(s.dynamic_size_, 9);
    EXPECT_EQ(std::string(s.c_str()), "01234");
    s.push_back('5');
    s.push_back('6');
    s.push_back('7');
    EXPECT_NE(s.dynamic_buffer_, nullptr);
    EXPECT_EQ(s.dynamic_size_, 9);
    EXPECT_EQ(std::string(s.c_str()), "01234567");
    s.push_back('8');
    EXPECT_NE(s.dynamic_buffer_, nullptr);
    EXPECT_EQ(s.dynamic_size_, 13);
    EXPECT_EQ(std::string(s.c_str()), "012345678");
}

} // namespace http

} // namespace rest
