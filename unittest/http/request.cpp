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

using namespace testing;

TEST(Request, ConstructionDestruction)
{
    std::shared_ptr<rest::http::Request> request;
    request = std::make_shared<rest::http::Request>(rest::socket::ConnectionPtr());
    EXPECT_EQ(request->m_connection, rest::socket::ConnectionPtr());
    EXPECT_EQ(request->m_buffer.size(), 0);
    EXPECT_EQ(request->m_method, rest::http::Method::UNKNOWN);
    EXPECT_EQ(request->m_url, std::string());
    EXPECT_EQ(request->m_version, rest::http::Version::HTTP_UNKNOWN);
    EXPECT_EQ(request->m_headers.size(), 0);
    EXPECT_EQ(request->m_data.size(), 0);
}

TEST(Request, Methods)
{
    EXPECT_EQ(rest::http::Request::parseMethod("OPTIONS"), rest::http::Method::OPTIONS);
    EXPECT_EQ(rest::http::Request::parseMethod("GET"), rest::http::Method::GET);
    EXPECT_EQ(rest::http::Request::parseMethod("HEAD"), rest::http::Method::HEAD);
    EXPECT_EQ(rest::http::Request::parseMethod("POST"), rest::http::Method::POST);
    EXPECT_EQ(rest::http::Request::parseMethod("PUT"), rest::http::Method::PUT);
    EXPECT_EQ(rest::http::Request::parseMethod("DELETE"), rest::http::Method::DELETE);
    EXPECT_EQ(rest::http::Request::parseMethod("TRACE"), rest::http::Method::TRACE);
    EXPECT_EQ(rest::http::Request::parseMethod("CONNECT"), rest::http::Method::CONNECT);
    EXPECT_THROW(rest::http::Request::parseMethod("???"), std::exception);
}

TEST(Request, Version)
{
    EXPECT_EQ(rest::http::Request::parseVersion("HTTP/1.0"), rest::http::Version::HTTP_1_0);
    EXPECT_EQ(rest::http::Request::parseVersion("HTTP/1.1"), rest::http::Version::HTTP_1_1);
    EXPECT_THROW(rest::http::Request::parseVersion("???"), std::exception);
}

TEST(Request, SetAndDeliver)
{
    std::shared_ptr<rest::socket::MockConnectionSocket> socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::http::Request request(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(1)
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        std::stringstream str;
        str << "GET / HTTP/1.1\r\n";
        str << "Content-Length: 1\r\n";
        str << "Accept: text/html,\r\n";
        str << " text/xml\r\n";
        str << "\r\n";
        str << "0";
        std::string reqData = str.str();
        data = rest::Buffer(reqData.begin(), reqData.end());
        return true;
    }));
    request.parse();

    EXPECT_EQ(request.header("Content-Length"), "1");
    EXPECT_THROW(request.header("???"), std::exception);
    EXPECT_EQ(request.m_headers.size(), 2);
    EXPECT_EQ(request.data(), rest::Buffer({ '0' }));
    EXPECT_EQ(request.method(), rest::http::Method::GET);
    EXPECT_EQ(request.url(), "/");
    EXPECT_EQ(request.version(), rest::http::Version::HTTP_1_1);
}

TEST(Request, NoNeededDataAvailable)
{
    std::shared_ptr<rest::socket::MockConnectionSocket> socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::http::Request request(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(1)
    .WillOnce(Return(false));
    EXPECT_THROW(request.parse(), std::exception);
}
