/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

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

#include <http/request.hpp>

#include <socket/mocksocketinterface.hpp>
#include <socket/inputsocketstream.hpp>

using namespace testing;

TEST(InputSocketStream, ConstructionDestruction)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();

    rest::socket::InputSocketStream iss(socket);

    EXPECT_EQ(iss.m_flags.eof, false);
    EXPECT_EQ(iss.m_flags.sane, true);
    EXPECT_EQ(iss.m_flags.error, false);
}

TEST(InputSocketStream, EmptyConnection)
{
    rest::socket::MockConnectionPtr socket;
    rest::socket::InputSocketStream iss(socket);

    EXPECT_EQ(iss.m_flags.eof, false);
    EXPECT_EQ(iss.m_flags.sane, false);
    EXPECT_EQ(iss.m_flags.error, true);
    EXPECT_EQ(iss.get(), '\0');
}

TEST(InputSocketStream, Get)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::socket::InputSocketStream iss(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(1)
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        return true;
    }));
    for (size_t i = 0; i < 10; i++)
    {
        EXPECT_EQ(iss.get(), static_cast<char>(i));
    }
}

TEST(InputSocketStream, GetViaStreamOperator)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::socket::InputSocketStream iss(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(1)
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        return true;
    }));
    for (size_t i = 0; i < 10; i++)
    {
        char c;
        iss >> c;
        EXPECT_EQ(c, static_cast<char>(i));
    }
}

TEST(InputSocketStream, GetEof)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::socket::InputSocketStream iss(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(1)
    .WillOnce(Invoke([](rest::Buffer & /*data*/, const size_t & /*maxSize*/) -> bool
    {
        return false;
    }));

    EXPECT_EQ(iss.get(), '\0');
    EXPECT_EQ(iss.get(), '\0');
    EXPECT_EQ(iss.m_flags.eof, true);
    EXPECT_EQ(iss.m_flags.sane, false);
    EXPECT_EQ(iss.m_flags.error, false);
}

TEST(InputSocketStream, Unget)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::socket::InputSocketStream iss(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(1)
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        return true;
    }));
    EXPECT_EQ(iss.get(), 0);
    iss.m_index = 4;
    iss.unget();
    EXPECT_EQ(iss.get(), 3);
}

TEST(InputSocketStream, StringNull)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::socket::InputSocketStream iss(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(2)
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        std::string reqData = "abc";
        data = rest::Buffer(reqData.begin(), reqData.end());
        return true;
    }))
    .WillOnce(Invoke([](rest::Buffer & /*data*/, const size_t & /*maxSize*/) -> bool
    {
        return false;
    }));

    std::string s;
    iss >> s;
    EXPECT_EQ(s, "abc");
}

TEST(InputSocketStream, StringNewline)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::socket::InputSocketStream iss(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(2)
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        std::string reqData = "abc\ndef";
        data = rest::Buffer(reqData.begin(), reqData.end());
        return true;
    }))
    .WillOnce(Invoke([](rest::Buffer & /*data*/, const size_t & /*maxSize*/) -> bool
    {
        return false;
    }));

    std::string s;
    iss >> s;
    EXPECT_EQ(s, "abc");
    iss >> s;
    EXPECT_EQ(s, "def");
}

TEST(InputSocketStream, StringCarriageReturn)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::socket::InputSocketStream iss(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(2)
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        std::string reqData = "abc\rdef";
        data = rest::Buffer(reqData.begin(), reqData.end());
        return true;
    }))
    .WillOnce(Invoke([](rest::Buffer & /*data*/, const size_t & /*maxSize*/) -> bool
    {
        return false;
    }));

    std::string s;
    iss >> s;
    EXPECT_EQ(s, "abc");
    iss >> s;
    EXPECT_EQ(s, "def");
}

TEST(InputSocketStream, StringCarriageReturnNewline)
{
    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::socket::InputSocketStream iss(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(2)
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        std::string reqData = "abc\r\ndef";
        data = rest::Buffer(reqData.begin(), reqData.end());
        return true;
    }))
    .WillOnce(Invoke([](rest::Buffer & /*data*/, const size_t & /*maxSize*/) -> bool
    {
        return false;
    }));

    std::string s;
    iss >> s;
    EXPECT_EQ(s, "abc");
    iss >> s;
    EXPECT_EQ(s, "def");
}
