/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utility/parsing.hpp"

using namespace testing;

namespace hutzn
{

TEST(parsing, skip_whitespace_null)
{
    const char_t* data = NULL;
    size_t remaining = 0;

    skip_whitespace(data, remaining);

    EXPECT_STREQ(NULL, data);
    EXPECT_EQ(0, remaining);
}

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

TEST(parsing, skip_one_character_null)
{
    const char_t* data = NULL;
    size_t remaining = 0;

    skip_one_character(data, remaining);

    EXPECT_STREQ(NULL, data);
    EXPECT_EQ(0, remaining);
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

TEST(parsing, parse_unsigned_interger_null)
{
    const char_t* data = NULL;
    size_t remaining = 0;

    EXPECT_EQ(-1, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ(NULL, data);
    EXPECT_EQ(0, remaining);
}

TEST(parsing, parse_unsigned_interger_nothing)
{
    std::string str = "";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(-1, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ(str.c_str(), data);
    EXPECT_EQ(str.size(), remaining);
}

TEST(parsing, parse_unsigned_interger_space)
{
    std::string str = " ";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(-1, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ(str.c_str(), data);
    EXPECT_EQ(str.size(), remaining);
}

TEST(parsing, parse_unsigned_interger_letter)
{
    std::string str = "a";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(-1, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ(str.c_str(), data);
    EXPECT_EQ(str.size(), remaining);
}

TEST(parsing, parse_unsigned_interger_zero)
{
    std::string str = "0";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(0, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ("", data);
    EXPECT_EQ(0, remaining);
}

TEST(parsing, parse_unsigned_interger_zero_stop_due_to_space)
{
    std::string str = "0 ";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(0, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ(" ", data);
    EXPECT_EQ(1, remaining);
}

TEST(parsing, parse_unsigned_interger_zero_stop_due_to_letter)
{
    std::string str = "0a";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(0, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ("a", data);
    EXPECT_EQ(1, remaining);
}

TEST(parsing, parse_unsigned_interger_zero_stop_due_to_overflow)
{
    std::string str = "3000000000";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(-2, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ("", data);
    EXPECT_EQ(0, remaining);
}

TEST(parsing, parse_unsigned_interger_zero_stop_due_to_overflow_64bit)
{
    std::string str = "30000000000000000000";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(-2, parse_unsigned_integer<int64_t>(data, remaining));
    EXPECT_STREQ("", data);
    EXPECT_EQ(0, remaining);
}

TEST(parsing, parse_unsigned_interger_useful_number)
{
    std::string str = "42";
    const char_t* data = str.c_str();
    size_t remaining = str.size();

    EXPECT_EQ(42, parse_unsigned_integer<int32_t>(data, remaining));
    EXPECT_STREQ("", data);
    EXPECT_EQ(0, remaining);
}

} // namespace hutzn
