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
#include <socket/connectionsocket.hpp>

using namespace testing;

namespace
{

void disable_time_wait(int socket)
{
    ::linger l = ::linger {1, 0};
    ::setsockopt(socket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

int get_socket(const rest::http::server_pointer & server)
{
    auto listener = std::dynamic_pointer_cast<rest::http::server>(server)->socket_;
    return std::dynamic_pointer_cast<rest::socket::listener_socket>(listener)->socket_;
}

int get_socket(const rest::socket::connection_pointer & connection)
{
    return std::dynamic_pointer_cast<rest::socket::connection_socket>(connection)->socket_;
}

int get_socket(const rest::socket::listener_pointer & listener)
{
    return std::dynamic_pointer_cast<rest::socket::listener_socket>(listener)->socket_;
}

}

TEST(server, construction_destruction)
{
    auto s = rest::http::create_server("127.0.0.1", 10000, rest::http::transaction_function());
    disable_time_wait(get_socket(s));
}

TEST(server, normal_use_case)
{
    bool called = false;
    auto transaction = [&called](const rest::http::request_interface & /*request*/,
    rest::http::response_interface & /*response*/) {
        called = true;
    };

    auto server = rest::http::create_server("localhost", 10000, transaction);
    disable_time_wait(get_socket(server));

    std::thread thread(std::bind(&rest::http::server_interface::run, server.get()));

    rest::socket::connection_pointer connection = rest::socket::connect("localhost", 10000);
    EXPECT_TRUE(connection->connect());
    disable_time_wait(get_socket(connection));
    std::string request = "GET / HTTP/1.1\r\n\r\n";
    EXPECT_FALSE(called);
    EXPECT_TRUE(connection->send(request));
    rest::buffer data;
    EXPECT_TRUE(connection->receive(data, 4000));
    EXPECT_TRUE(called);

    server->stop();
    thread.join();
}
