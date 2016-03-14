/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#include "libhutznohmd/mock_communication.hpp"
#include "request/lexer.hpp"

using namespace testing;

namespace hutzn
{

class lexer_test : public Test
{
protected:
    void check(const std::string& chunk, const std::string& result,
               const bool fetch_result)
    {
        const connection_mock_ptr conn = std::make_shared<connection_mock>();
        EXPECT_CALL(*conn, receive(_, _))
            .Times(AtLeast(1))
            .WillOnce(Invoke([chunk](buffer& b, const size_t& m) {
                EXPECT_LE(chunk.size(), m);
                b.insert(b.begin(), chunk.begin(), chunk.end());
                return true;
            }))
            .WillRepeatedly(Return(false));

        lexer lex(conn);
        EXPECT_EQ(fetch_result, lex.fetch_header());
        for (size_t i = 0; i < result.size(); i++) {
            EXPECT_EQ(i, lex.index());
            EXPECT_EQ(static_cast<uint8_t>(result[i]), lex.get());
        }
        EXPECT_EQ(result.size(), lex.index());
        EXPECT_EQ(-1, lex.get());

        for (size_t i = 0; i < result.size(); i++) {
            char_t* ch = lex.header_data(i);
            const char_t* ch2 = const_cast<const lexer&>(lex).header_data(i);
            ASSERT_TRUE(NULL != ch);
            EXPECT_EQ(result[i], *ch);
            EXPECT_EQ(ch, ch2);
        }
        EXPECT_EQ(NULL, lex.header_data(result.size()));
        EXPECT_EQ(NULL,
                  const_cast<const lexer&>(lex).header_data(result.size()));

        const char_t* data = lex.header_data(0);
        const char_t* data2 = const_cast<const lexer&>(lex).header_data(0);
        ASSERT_TRUE(NULL != data);
        ASSERT_EQ(data, data2);
        for (size_t i = 0; i < result.size(); i++) {
            EXPECT_EQ(static_cast<uint8_t>(result[i]), data[i]);
        }
    }
};

TEST_F(lexer_test, construction)
{
    const connection_mock_ptr conn = std::make_shared<connection_mock>();
    EXPECT_CALL(*conn, receive(_, _)).Times(1).WillOnce(Return(false));

    lexer lex(conn);
    EXPECT_EQ(NULL, lex.header_data(0));
    EXPECT_EQ(-1, lex.get());
    EXPECT_FALSE(lex.fetch_header());
}

TEST_F(lexer_test, call_receive_successfully_once)
{
    const std::string chunk = "GET / HTTP/1.1\n\n";
    check(chunk, chunk, true);
}

TEST_F(lexer_test, several_lws_tokens)
{
    const std::string chunk = "a\n b\r\n c\r d\n\te\r\n\tf\r\tg";
    const std::string result = "a b c d e f g";
    check(chunk, result, false);
}

TEST_F(lexer_test, newline_token_1)
{
    const std::string chunk = "a\nb\r\nc\rd";
    const std::string result = "a\nb\nc\nd";
    check(chunk, result, false);
}

TEST_F(lexer_test, newline_token_2)
{
    const std::string chunk = "a\r\n\r\nb";
    const std::string result = "a\n\n";
    check(chunk, result, true);
}

TEST_F(lexer_test, newline_token_3)
{
    const std::string chunk = "a\r\rb";
    const std::string result = "a\n\n";
    check(chunk, result, true);
}

TEST_F(lexer_test, newline_token_4)
{
    const std::string chunk = "a\n\rb";
    const std::string result = "a\n\n";
    check(chunk, result, true);
}

TEST_F(lexer_test, newline_token_5)
{
    const std::string chunk = "a\n\r\nb";
    const std::string result = "a\n\n";
    check(chunk, result, true);
}

TEST_F(lexer_test, newline_token_6)
{
    const std::string chunk = "a\r\n\rb";
    const std::string result = "a\n\n";
    check(chunk, result, true);
}

TEST_F(lexer_test, set_index)
{
    const std::string chunk = "abcdefgh";
    const connection_mock_ptr conn = std::make_shared<connection_mock>();
    EXPECT_CALL(*conn, receive(_, _))
        .Times(2)
        .WillOnce(Invoke([chunk](buffer& b, const size_t& m) {
            EXPECT_LE(chunk.size(), m);
            b.insert(b.begin(), chunk.begin(), chunk.end());
            return true;
        }))
        .WillOnce(Return(false));

    lexer lex(conn);
    EXPECT_FALSE(lex.fetch_header());
    ASSERT_EQ(0, lex.index());
    EXPECT_EQ('a', lex.get());
    EXPECT_EQ(1, lex.index());
    lex.set_index(4);
    EXPECT_EQ('e', lex.get());
    EXPECT_EQ(5, lex.index());
    lex.set_index(0);
    EXPECT_EQ('a', lex.get());
    EXPECT_EQ(1, lex.index());
}

TEST_F(lexer_test, set_index_beyond_border)
{
    const std::string chunk = "abcdefgh";
    const connection_mock_ptr conn = std::make_shared<connection_mock>();
    EXPECT_CALL(*conn, receive(_, _))
        .Times(2)
        .WillOnce(Invoke([chunk](buffer& b, const size_t& m) {
            EXPECT_LE(chunk.size(), m);
            b.insert(b.begin(), chunk.begin(), chunk.end());
            return true;
        }))
        .WillOnce(Return(false));

    lexer lex(conn);
    EXPECT_FALSE(lex.fetch_header());
    ASSERT_EQ(0, lex.index());
    EXPECT_EQ('a', lex.get());
    EXPECT_EQ(1, lex.index());
    lex.set_index(9);
    EXPECT_EQ(1, lex.index());
    EXPECT_EQ('b', lex.get());
    EXPECT_EQ(2, lex.index());
}

TEST_F(lexer_test, set_index_to_eof)
{
    const std::string chunk = "abcdefgh";
    const connection_mock_ptr conn = std::make_shared<connection_mock>();
    EXPECT_CALL(*conn, receive(_, _))
        .Times(2)
        .WillOnce(Invoke([chunk](buffer& b, const size_t& m) {
            EXPECT_LE(chunk.size(), m);
            b.insert(b.begin(), chunk.begin(), chunk.end());
            return true;
        }))
        .WillOnce(Return(false));

    lexer lex(conn);
    EXPECT_FALSE(lex.fetch_header());
    ASSERT_EQ(0, lex.index());
    EXPECT_EQ('a', lex.get());
    EXPECT_EQ(1, lex.index());
    lex.set_index(8);
    EXPECT_EQ(-1, lex.get());
    EXPECT_EQ(8, lex.index());
    lex.set_index(0);
    EXPECT_EQ('a', lex.get());
    EXPECT_EQ(1, lex.index());
}

TEST_F(lexer_test, content)
{
    const std::string chunk1 = "\r\n\r\n";
    const std::string chunk2 = "a";
    const std::string chunk3 = "b";
    const connection_mock_ptr conn = std::make_shared<connection_mock>();
    EXPECT_CALL(*conn, receive(_, _))
        .Times(4)
        .WillOnce(Invoke([chunk1](buffer& b, const size_t& m) {
            EXPECT_LE(chunk1.size(), m);
            b.insert(b.end(), chunk1.begin(), chunk1.end());
            return true;
        }))
        .WillOnce(Invoke([chunk2](buffer& b, const size_t& m) {
            EXPECT_LE(chunk2.size(), m);
            b.insert(b.end(), chunk2.begin(), chunk2.end());
            return true;
        }))
        .WillOnce(Invoke([chunk3](buffer& b, const size_t& m) {
            EXPECT_LE(chunk3.size(), m);
            b.insert(b.end(), chunk3.begin(), chunk3.end());
            return true;
        }))
        .WillOnce(Return(false));

    lexer lex(conn);

    // Initially the content is empty..
    EXPECT_EQ(0, lex.content_length());
    EXPECT_EQ(NULL, lex.content());

    // Fetching the header will not effect the content.
    EXPECT_TRUE(lex.fetch_header());
    EXPECT_EQ(0, lex.content_length());
    EXPECT_EQ(NULL, lex.content());

    // Content is available after calling fetch_content.
    EXPECT_TRUE(lex.fetch_content(2));
    EXPECT_EQ(2, lex.content_length());
    EXPECT_EQ('a', lex.content()[0]);
    EXPECT_EQ('b', lex.content()[1]);

    // Fetching too much will invalidate the content.
    EXPECT_FALSE(lex.fetch_content(3));
    EXPECT_EQ(0, lex.content_length());
    EXPECT_EQ(NULL, lex.content());
}

} // namespace hutzn
