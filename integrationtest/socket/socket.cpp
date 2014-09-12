/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <thread>

#include <gtest/gtest.h>

#include <socket/connection_socket.hpp>
#include <socket/listener_socket.hpp>

namespace rest
{

namespace
{

void disable_time_wait(int socket)
{
    ::linger l = ::linger {1, 0};
    ::setsockopt(socket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

int get_socket(const socket::connection_pointer & connection)
{
    return std::dynamic_pointer_cast<socket::connection_socket>(connection)->socket();
}

int get_socket(const socket::listener_pointer & listener)
{
    return std::dynamic_pointer_cast<socket::listener_socket>(listener)->socket();
}

}

TEST(socket, construction_no_throw)
{
    auto listener = socket::listener_socket::create("127.0.0.1", 10000);
    disable_time_wait(listener->socket());
    EXPECT_NE(listener, std::shared_ptr<socket::listener_socket>());
    EXPECT_TRUE(listener->listening());
}

TEST(socket, wrong_construction_arguments)
{
    auto listener = socket::listen("127.0.0.1", 10000);
    disable_time_wait(get_socket(listener));

    EXPECT_EQ(socket::listen("127.0.0.1", 10000), socket::listener_pointer());
    EXPECT_TRUE(listener->listening());
}

TEST(socket, accepting_closed_socket)
{
    auto listener = socket::listener_socket::create("127.0.0.1", 10000);
    disable_time_wait(listener->socket());
    EXPECT_EQ(::close(listener->socket()), 0);
    EXPECT_EQ(listener->accept(), socket::connection_pointer());
}

TEST(socket, connecting_closed_socket)
{
    auto connection = socket::connection_socket::create("127.0.0.1", 10000);
    connection->close();
    EXPECT_FALSE(connection->connect());
}

TEST(socket, connection_refused)
{
    auto connection = socket::connection_socket::create("127.0.0.1", 10000);
    EXPECT_FALSE(connection->connect());
}

TEST(socket, receive_send_closed_socket)
{
    auto listener = socket::listen("127.0.0.1", 10000);
    disable_time_wait(get_socket(listener));
    EXPECT_TRUE(listener->listening());

    bool connected = false;
    bool disconnected = false;
    std::thread thread([&disconnected, &connected] {
        auto connection = socket::connection_socket::create("127.0.0.1", 10000);
        EXPECT_TRUE(connection->connect());
        disable_time_wait(connection->socket());
        connected = true;
        while (disconnected == false) {
            usleep(1);
        }

        buffer data;
        EXPECT_FALSE(connection->receive(data, 8));

        EXPECT_EQ(::close(connection->socket()), 0);
        EXPECT_FALSE(connection->receive(data, 8));
        EXPECT_FALSE(connection->send(data));
    });

    socket::connection_pointer connection = listener->accept();
    disable_time_wait(get_socket(connection));
    while (connected == false) {
        usleep(1);
    }
    connection.reset();
    disconnected = true;
    thread.join();
    EXPECT_TRUE(listener->listening());
}

TEST(socket, double_connect)
{
    socket::listener_pointer listener = socket::listen("127.0.0.1", 10000);
    disable_time_wait(get_socket(listener));
    EXPECT_TRUE(listener->listening());

    std::thread thread([] {
        auto connection = socket::connection_socket::create("127.0.0.1", 10000);
        EXPECT_TRUE(connection->connect());
        EXPECT_FALSE(connection->connect());
        disable_time_wait(connection->socket());
    });

    socket::connection_pointer connection = listener->accept();
    disable_time_wait(get_socket(connection));
    thread.join();
    EXPECT_TRUE(listener->listening());
}

TEST(socket, unconnected_send_receive)
{
    auto connection = socket::connection_socket::create("127.0.0.1", 10000);
    buffer data;
    EXPECT_FALSE(connection->send(""));
    EXPECT_FALSE(connection->receive(data, 0));
}

TEST(socket, terminate_try_to_connect)
{
    auto connection = socket::connection_socket::create("240.0.0.1", 65535);

    std::thread thread([&connection] {
        EXPECT_FALSE(connection->connect());
    });

    usleep(10000);
    connection->close();
    thread.join();
}

TEST(socket, terminate_try_to_accept)
{
    socket::listener_pointer listener = socket::listen("127.0.0.1", 10000);
    disable_time_wait(get_socket(listener));
    EXPECT_TRUE(listener->listening());

    std::thread thread([&listener] {
        EXPECT_EQ(listener->accept(), socket::connection_pointer());
        EXPECT_FALSE(listener->listening());
    });

    usleep(10000);
    EXPECT_TRUE(listener->listening());
    listener->stop();
    thread.join();
    EXPECT_FALSE(listener->listening());
}

TEST(socket, normal_use_case)
{
    socket::listener_pointer listener = socket::listen("127.0.0.1", 10000);
    disable_time_wait(get_socket(listener));
    EXPECT_TRUE(listener->listening());

    std::thread thread([] {
        socket::connection_pointer connection = socket::connect("127.0.0.1", 10000);
        EXPECT_TRUE(connection->connect());
        EXPECT_NE(connection, socket::connection_pointer());
        disable_time_wait(get_socket(connection));
        buffer data;
        EXPECT_TRUE(connection->receive(data, 8));
        EXPECT_EQ(data, buffer({ 0, 1, 2, 3 }));
        data = { 4, 5, 6, 7 };
        EXPECT_TRUE(connection->send(data));
    });

    socket::connection_pointer connection = listener->accept();
    EXPECT_NE(connection, socket::connection_pointer());
    disable_time_wait(get_socket(connection));
    buffer data = { 0, 1, 2, 3 };
    EXPECT_TRUE(connection->send(data));
    data.clear();
    EXPECT_TRUE(connection->receive(data, 8));
    EXPECT_EQ(data, buffer({ 4, 5, 6, 7 }));

    thread.join();
    EXPECT_TRUE(listener->listening());
}

} // namespace rest
