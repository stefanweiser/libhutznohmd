/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/server.hpp>
#include <socket/connectionsocket.hpp>

using namespace testing;

namespace rest
{

namespace
{

void disable_time_wait(int socket)
{
    ::linger l = ::linger {1, 0};
    ::setsockopt(socket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

int get_socket(const http::server_pointer & server)
{
    auto listener = std::dynamic_pointer_cast<http::server>(server)->socket_;
    return std::dynamic_pointer_cast<socket::listener_socket>(listener)->socket_;
}

int get_socket(const socket::connection_pointer & connection)
{
    return std::dynamic_pointer_cast<socket::connection_socket>(connection)->socket_;
}

int get_socket(const socket::listener_pointer & listener)
{
    return std::dynamic_pointer_cast<socket::listener_socket>(listener)->socket_;
}

}

TEST(server, construction_destruction)
{
    auto s = http::create_server("127.0.0.1", 10000, http::transaction_function());
    disable_time_wait(get_socket(s));
}

TEST(server, normal_use_case_http_1_1)
{
    bool called = false;
    auto transaction = [&called](const http::request_interface & /*request*/,
    http::response_interface & /*response*/) {
        called = true;
    };

    auto server = http::create_server("localhost", 10000, transaction);
    disable_time_wait(get_socket(server));

    std::thread thread(std::bind(&http::server_interface::run, server.get()));

    socket::connection_pointer connection = socket::connect("localhost", 10000);
    EXPECT_TRUE(connection->connect());
    disable_time_wait(get_socket(connection));
    std::string request = "GET / HTTP/1.1\r\n\r\n";
    EXPECT_FALSE(called);
    EXPECT_TRUE(connection->send(request));
    buffer data;
    EXPECT_TRUE(connection->receive(data, 4000));
    EXPECT_TRUE(called);

    server->stop();
    thread.join();
}

TEST(server, normal_use_case_http_1_0)
{
    bool called = false;
    auto transaction = [&called](const http::request_interface & /*request*/,
    http::response_interface & /*response*/) {
        called = true;
    };

    auto server = http::create_server("localhost", 10000, transaction);
    disable_time_wait(get_socket(server));

    std::thread thread(std::bind(&http::server_interface::run, server.get()));

    socket::connection_pointer connection = socket::connect("localhost", 10000);
    EXPECT_TRUE(connection->connect());
    disable_time_wait(get_socket(connection));
    std::string request = "GET / HTTP/1.0\r\n\r\n";
    EXPECT_FALSE(called);
    EXPECT_TRUE(connection->send(request));
    buffer data;
    EXPECT_TRUE(connection->receive(data, 4000));
    EXPECT_TRUE(called);

    server->stop();
    thread.join();
}

} // namespace rest
