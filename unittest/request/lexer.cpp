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

#include <libhutznohmd/mock_sockets.hpp>

#include <request/lexer.hpp>

using namespace testing;

namespace hutzn
{

class lexer_test : public ::testing::Test
{
public:
    void SetUp(void) override
    {
    }

    void TearDown(void) override
    {
    }

protected:
    void check(const std::string& chunk, const std::string& result)
    {
        connection_mock_pointer c =
            std::make_shared<connection_interface_mock>();
        EXPECT_CALL(*c, receive(_, _))
            .Times(2)
            .WillOnce(Invoke([&chunk](buffer& b, const size_t& m) {
                EXPECT_LE(chunk.size(), m);
                b.insert(b.begin(), chunk.begin(), chunk.end());
                return true;
            }))
            .WillOnce(Return(false));

        lexer l(c);
        for (size_t i = 0; i < result.size(); i++) {
            EXPECT_EQ(i, l.index());
            EXPECT_EQ(static_cast<uint8_t>(result[i]), l.get());
        }
        EXPECT_EQ(result.size(), l.index());
        EXPECT_EQ(-1, l.get());

        for (size_t i = 0; i < result.size(); i++) {
            const char_t* ch = l.data(i);
            ASSERT_NE(nullptr, ch);
            EXPECT_EQ(result[i], *ch);
        }
        EXPECT_EQ(nullptr, l.data(result.size()));

        const char_t* data = l.data(0);
        ASSERT_NE(nullptr, data);
        for (size_t i = 0; i < result.size(); i++) {
            EXPECT_EQ(static_cast<uint8_t>(result[i]), data[i]);
        }
    }
};

TEST_F(lexer_test, construction)
{
    connection_mock_pointer c = std::make_shared<connection_interface_mock>();
    EXPECT_CALL(*c, receive(_, _)).Times(1).WillOnce(Return(false));

    lexer l(c);
    EXPECT_EQ(nullptr, l.data(0));
    EXPECT_EQ(-1, l.get());
}

TEST_F(lexer_test, call_receive_successfully_once)
{
    std::string chunk = "GET / HTTP/1.1\n\n";
    check(chunk, chunk);
}

TEST_F(lexer_test, several_lws_tokens)
{
    std::string chunk = "a\n b\r\n c\r d\n\te\r\n\tf\r\tg";
    std::string result = "a b c d e f g";
    check(chunk, result);
}

TEST_F(lexer_test, newline_token_1)
{
    std::string chunk = "a\nb\r\nc\rd\n\n";
    std::string result = "a\nb\nc\nd\n\n";
    check(chunk, result);
}

TEST_F(lexer_test, newline_token_2)
{
    std::string chunk = "a\r\n\r\nb";
    std::string result = "a\n\n";
    check(chunk, result);
}

TEST_F(lexer_test, newline_token_3)
{
    std::string chunk = "a\r\rb";
    std::string result = "a\n\n";
    check(chunk, result);
}

TEST_F(lexer_test, newline_token_4)
{
    std::string chunk = "a\n\rb";
    std::string result = "a\n\n";
    check(chunk, result);
}

TEST_F(lexer_test, newline_token_5)
{
    std::string chunk = "a\n\r\nb";
    std::string result = "a\n\n";
    check(chunk, result);
}

TEST_F(lexer_test, newline_token_6)
{
    std::string chunk = "a\r\n\rb";
    std::string result = "a\n\n";
    check(chunk, result);
}

} // namespace hutzn
