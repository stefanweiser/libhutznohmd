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

#include <http/_server.hpp>

#include <socket/mocksocketinterface.hpp>

using namespace testing;

TEST(Server, Accept)
{
    std::shared_ptr<rest::socket::MockListenerSocket> socket;
    socket = std::make_shared<rest::socket::MockListenerSocket>();
    rest::http::Server server(socket, rest::http::AccessFn());
    std::shared_ptr<rest::socket::MockConnectionSocket> connection;
    connection = std::make_shared<rest::socket::MockConnectionSocket>();
    EXPECT_CALL(*socket, accept())
    .Times(1)
    .WillOnce(Return(connection));

    rest::http::Server::Connection result = server.accept();

    EXPECT_EQ(connection, result);
}

TEST(Server, Request)
{
    std::shared_ptr<rest::socket::MockListenerSocket> socket;
    socket = std::make_shared<rest::socket::MockListenerSocket>();
    rest::http::Server server(socket, rest::http::AccessFn());
    std::shared_ptr<rest::socket::MockConnectionSocket> connection;
    connection = std::make_shared<rest::socket::MockConnectionSocket>();
    EXPECT_CALL(*connection, receive(_))
    .Times(1)
    .WillOnce(Invoke([](std::vector<uint8_t> & data) -> bool
    {
        data = { '0', '1', '2', '3' };
        return true;
    }));
    server.parseRequest(connection);
}
