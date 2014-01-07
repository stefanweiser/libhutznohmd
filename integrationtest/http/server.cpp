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

#include <http/server.hpp>
#include <socket/connectionsocket.hpp>

using namespace testing;

namespace
{

void disableTimeWait(int socket)
{
    ::linger l = ::linger {1, 0};
    ::setsockopt(socket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

int getSocket(const rest::http::ServerPtr & server)
{
    auto listener = std::dynamic_pointer_cast<rest::http::Server>(server)->m_socket;
    return std::dynamic_pointer_cast<rest::socket::ListenerSocket>(listener)->m_socket;
}

int getSocket(const rest::socket::ConnectionPtr & connection)
{
    return std::dynamic_pointer_cast<rest::socket::ConnectionSocket>(connection)->m_socket;
}

int getSocket(const rest::socket::ListenerPtr & listener)
{
    return std::dynamic_pointer_cast<rest::socket::ListenerSocket>(listener)->m_socket;
}

}

TEST(Server, ConstructionDestruction)
{
    rest::http::ServerPtr server;
    server = rest::http::createServer("127.0.0.1", 10000, rest::http::TransactionFn());
    disableTimeWait(getSocket(server));
}

TEST(Server, NormalUseCase)
{
    bool called = false;
    auto transaction = [&called](
                           const rest::http::RequestInterface & /*request*/,
                           rest::http::ResponseInterface & /*response*/)
    {
        called = true;
    };

    rest::http::ServerPtr server;
    server = rest::http::createServer("localhost", 10001, transaction);
    disableTimeWait(getSocket(server));

    std::thread t(std::bind(&rest::http::ServerInterface::run, server.get()));

    rest::socket::ConnectionPtr connection = rest::socket::connect("localhost", 10001);
    disableTimeWait(getSocket(connection));
    std::string request = "GET / HTTP/1.1\r\n\r\n";
    EXPECT_FALSE(called);
    EXPECT_TRUE(connection->send(rest::Buffer(request.begin(), request.end())));
    rest::Buffer data;
    EXPECT_TRUE(connection->receive(data, 4000));
    EXPECT_TRUE(called);

    server->stop();
    t.join();
}
