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

#include <http/parser/utility/lexer.hpp>

using namespace testing;

namespace
{

int anonymous_get(void * handle)
{
    return static_cast<std::istream *>(handle)->get();
}

int anonymous_peek(void * handle)
{
    return static_cast<std::istream *>(handle)->peek();
}

}

TEST(lexer, empty)
{
    std::stringstream s("");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get(), -1);
    EXPECT_EQ(l.peek(), -1);
}

TEST(lexer, string)
{
    std::stringstream s("ab");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get(), 'a');
    EXPECT_EQ(l.peek(), 'b');
    EXPECT_EQ(l.get(), 'b');
    EXPECT_EQ(l.peek(), -1);
}

TEST(lexer, newline)
{
    std::stringstream s("a\nb");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get(), 'a');
    EXPECT_EQ(l.peek(), '\n');
    EXPECT_EQ(l.get(), '\n');
    EXPECT_EQ(l.peek(), 'b');
}

TEST(lexer, carriage_return_newline)
{
    std::stringstream s("a\r\nb");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get(), 'a');
    EXPECT_EQ(l.peek(), '\r');
    EXPECT_EQ(l.get(), '\n');
    EXPECT_EQ(l.peek(), 'b');
}

TEST(lexer, carriage_return)
{
    std::stringstream s("a\rb");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get(), 'a');
    EXPECT_EQ(l.peek(), '\r');
    EXPECT_EQ(l.get(), '\n');
    EXPECT_EQ(l.peek(), 'b');
}

TEST(lexer, space)
{
    std::stringstream s("a b");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get(), 'a');
    EXPECT_EQ(l.peek(), ' ');
    EXPECT_EQ(l.get(), ' ');
    EXPECT_EQ(l.peek(), 'b');
}

TEST(lexer, tabulator)
{
    std::stringstream s("a\tb");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get(), 'a');
    EXPECT_EQ(l.peek(), '\t');
    EXPECT_EQ(l.get(), '\t');
    EXPECT_EQ(l.peek(), 'b');
}

TEST(lexer, newline_whitespace)
{
    std::stringstream s("a\n\tb");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get(), 'a');
    EXPECT_EQ(l.peek(), '\n');
    EXPECT_EQ(l.get(), ' ');
    EXPECT_EQ(l.peek(), 'b');
}

TEST(lexer, next_non_whitespace)
{
    std::stringstream s(" \ta");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get_non_whitespace(), 'a');
}

TEST(lexer, newline_is_no_whitespace)
{
    std::stringstream s("\na");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get_non_whitespace(), '\n');
}

TEST(lexer, carriage_return_is_no_whitespace)
{
    std::stringstream s("\ra");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    EXPECT_EQ(l.get_non_whitespace(), '\n');
}

TEST(lexer, unsigned_integer)
{
    std::stringstream s("0123");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    int character = l.get();
    EXPECT_EQ(l.get_unsigned_integer(character), 123);
}

TEST(lexer, wrong_unsigned_integer)
{
    std::stringstream s("a");
    lexer l(anonymous_int_function(&anonymous_get, &s),
            anonymous_int_function(&anonymous_peek, &s));
    int character = l.get();
    EXPECT_EQ(l.get_unsigned_integer(character), -1);
}
