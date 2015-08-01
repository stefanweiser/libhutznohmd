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

#include <http/parser/utility/lexer.hpp>

using namespace testing;

namespace hutzn
{

namespace http
{

namespace
{

int32_t get_char(void* handle)
{
    return static_cast<std::istream*>(handle)->get();
}

int32_t peek_char(void* handle)
{
    return static_cast<std::istream*>(handle)->peek();
}

} // namespace

TEST(lexer, empty)
{
    std::stringstream s("");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ(-1, l.get());
}

TEST(lexer, string)
{
    std::stringstream s("ab");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('a', l.get());
    EXPECT_EQ('b', l.get());
}

TEST(lexer, newline)
{
    std::stringstream s("a\nb");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('a', l.get());
    EXPECT_EQ('\n', l.get());
}

TEST(lexer, carriage_return_newline)
{
    std::stringstream s("a\r\nb");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('a', l.get());
    EXPECT_EQ('\n', l.get());
}

TEST(lexer, carriage_return)
{
    std::stringstream s("a\rb");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('a', l.get());
    EXPECT_EQ('\n', l.get());
}

TEST(lexer, space)
{
    std::stringstream s("a b");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('a', l.get());
    EXPECT_EQ(' ', l.get());
}

TEST(lexer, tabulator)
{
    std::stringstream s("a\tb");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('a', l.get());
    EXPECT_EQ('\t', l.get());
}

TEST(lexer, newline_whitespace)
{
    std::stringstream s("a\n\tb");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('a', l.get());
    EXPECT_EQ(' ', l.get());
}

TEST(lexer, next_non_whitespace)
{
    std::stringstream s(" \ta");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('a', l.get_non_whitespace());
}

TEST(lexer, newline_is_no_whitespace)
{
    std::stringstream s("\na");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('\n', l.get_non_whitespace());
}

TEST(lexer, carriage_return_is_no_whitespace)
{
    std::stringstream s("\ra");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    EXPECT_EQ('\n', l.get_non_whitespace());
}

TEST(lexer, unsigned_integer)
{
    std::stringstream s("0123");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    int32_t character = l.get();
    EXPECT_EQ(123, l.get_unsigned_integer(character));
}

TEST(lexer, wrong_unsigned_integer)
{
    std::stringstream s("a");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    int32_t character = l.get();
    EXPECT_EQ(-1, l.get_unsigned_integer(character));
}

TEST(lexer, word)
{
    std::stringstream s("xyz ");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    int32_t character = l.get();
    push_back_string<4> data;
    parse_word(character, data, [](const uint8_t& c) { return c != ' '; }, l);
    EXPECT_EQ(std::string("xyz"), std::string(data.c_str()));
    EXPECT_EQ(' ', character);
}

TEST(lexer, quoted_string)
{
    std::stringstream s("\"xyz\"-");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    int32_t character = l.get();
    push_back_string<4> data;
    EXPECT_TRUE(parse_quoted_string(character, data, l));
    EXPECT_EQ(std::string("xyz"), std::string(data.c_str()));
    EXPECT_EQ('-', character);
}

TEST(lexer, quoted_string_error1)
{
    std::stringstream s("xyz");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    int32_t character = l.get();
    push_back_string<4> data;
    EXPECT_FALSE(parse_quoted_string(character, data, l));
    EXPECT_EQ(std::string(""), std::string(data.c_str()));
    EXPECT_EQ('x', character);
}

TEST(lexer, quoted_string_error2)
{
    std::stringstream s("\"xyz");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    int32_t character = l.get();
    push_back_string<4> data;
    EXPECT_FALSE(parse_quoted_string(character, data, l));
    EXPECT_EQ(std::string("xyz"), std::string(data.c_str()));
    EXPECT_EQ(-1, character);
}

TEST(lexer, comment)
{
    std::stringstream s("()-");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    int32_t character = l.get();
    EXPECT_TRUE(parse_comment(character, l));
    EXPECT_EQ('-', character);
}

TEST(lexer, nested_comments)
{
    std::stringstream s("(a()()b)-");
    lexer l(anonymous_int_function(&get_char, &s),
            anonymous_int_function(&peek_char, &s));
    int32_t character = l.get();
    EXPECT_TRUE(parse_comment(character, l));
    EXPECT_EQ('-', character);
}

} // namespace http

} // namespace hutzn
