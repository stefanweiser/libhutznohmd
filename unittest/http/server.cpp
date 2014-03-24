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

#include <http/server.hpp>

#include <socket/mocksocketinterface.hpp>

using namespace testing;

TEST(Server, ParsingRequest)
{
    bool called = false;
    auto transaction = [&called](
                           const rest::http::RequestInterface & /*request*/,
                           rest::http::ResponseInterface & /*response*/)
    {
        called = true;
    };
    rest::http::Server server(rest::socket::ListenerPtr(), transaction);

    rest::socket::MockConnectionPtr socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    EXPECT_CALL(*socket, receive(_, _))
    .WillOnce(Invoke([](rest::Buffer & data, const size_t & /*maxSize*/) -> bool
    {
        std::stringstream str;
        str << "GET / HTTP/1.1\r\n";
        str << "\r\n";
        std::string reqData = str.str();
        data = rest::Buffer(reqData.begin(), reqData.end());
        return true;
    }))
    .WillRepeatedly(Invoke([](rest::Buffer & /*data*/, const size_t & /*maxSize*/) -> bool
    {
        return false;
    }))
    ;
    EXPECT_CALL(*socket, send(An<const rest::Buffer &>()))
    .Times(1)
    .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string &>()))
    .Times(1)
    .WillRepeatedly(Return(true));
    EXPECT_EQ(called, false);
    server.parseRequest(socket);
    EXPECT_EQ(called, true);
}
