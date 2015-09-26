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

#include <utility/parsing.hpp>

using namespace testing;

namespace hutzn
{

TEST(parsing, skip_whitespace_nothing)
{
    std::string str = "abc";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    skip_whitespace(data, remaining);

    EXPECT_STREQ("abc", data);
    EXPECT_EQ(str.size(), remaining);
}

TEST(parsing, skip_whitespace_empty)
{
    std::string str = "";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    skip_whitespace(data, remaining);

    EXPECT_STREQ("", data);
    EXPECT_EQ(str.size(), remaining);
}

TEST(parsing, skip_whitespace_one_space)
{
    std::string str = " abc";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    skip_whitespace(data, remaining);

    EXPECT_STREQ("abc", data);
    EXPECT_EQ(str.size() - 1, remaining);
}

TEST(parsing, skip_whitespace_two_spaces)
{
    std::string str = "  abc";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    skip_whitespace(data, remaining);

    EXPECT_STREQ("abc", data);
    EXPECT_EQ(str.size() - 2, remaining);
}

TEST(parsing, skip_whitespace_one_tab)
{
    std::string str = "\tabc";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    skip_whitespace(data, remaining);

    EXPECT_STREQ("abc", data);
    EXPECT_EQ(str.size() - 1, remaining);
}

TEST(parsing, skip_whitespace_mixed_spaces_and_tabs)
{
    std::string str = " \t \tabc";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    skip_whitespace(data, remaining);

    EXPECT_STREQ("abc", data);
    EXPECT_EQ(str.size() - 4, remaining);
}

TEST(parsing, skip_one_character)
{
    std::string str = "abc";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    skip_one_character(data, remaining);

    EXPECT_STREQ("bc", data);
    EXPECT_EQ(str.size() - 1, remaining);
}

TEST(parsing, skip_one_character_nothing)
{
    std::string str = "";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    skip_one_character(data, remaining);

    EXPECT_STREQ("", data);
    EXPECT_EQ(str.size(), remaining);
}

} // namespace hutzn
