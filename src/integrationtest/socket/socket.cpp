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

#include <thread>

#include <gtest/gtest.h>

#include "socket/connection.hpp"

namespace hutzn
{

TEST(socket, listener_construction)
{
    auto listener = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listener->set_lingering_timeout(0));
    EXPECT_NE(listener_pointer(), listener);
    EXPECT_TRUE(listener->listening());
}

TEST(socket, wrong_construction_arguments)
{
    EXPECT_EQ(listener_pointer(), listen("127.0.0:1", 10000));
}

TEST(socket, reuse_host_and_port)
{
    auto listener = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listener->set_lingering_timeout(0));
    EXPECT_EQ(listener_pointer(), listen("127.0.0.1", 10000));
    EXPECT_TRUE(listener->listening());
}

TEST(socket, accepting_closed_socket)
{
    auto listener = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listener->set_lingering_timeout(0));
    listener->stop();
    EXPECT_EQ(connection_pointer(), listener->accept());
}

TEST(socket, connecting_closed_socket)
{
    auto connection = connection::create("127.0.0.1", 10000);
    connection->close();
    EXPECT_FALSE(connection->connect());
}

TEST(socket, connection_refused)
{
    auto connection = connection::create("127.0.0.1", 10000);
    EXPECT_FALSE(connection->connect());
}

TEST(socket, receive_send_closed_socket)
{
    auto listener = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listener->set_lingering_timeout(0));
    EXPECT_TRUE(listener->listening());

    bool connected = false;
    bool disconnected = false;
    std::thread thread([&disconnected, &connected] {
        auto connection = connection::create("127.0.0.1", 10000);
        EXPECT_TRUE(connection->connect());
        EXPECT_TRUE(connection->set_lingering_timeout(0));
        connected = true;
        while (disconnected == false) {
            usleep(1);
        }

        buffer data;
        EXPECT_FALSE(connection->receive(data, 8));

        connection->close();
        EXPECT_FALSE(connection->receive(data, 8));
        EXPECT_FALSE(connection->send(data));
    });

    connection_pointer connection = listener->accept();
    EXPECT_TRUE(connection->set_lingering_timeout(0));
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
    listener_pointer listener = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listener->set_lingering_timeout(0));
    EXPECT_TRUE(listener->listening());

    std::thread thread([] {
        auto connection = connection::create("127.0.0.1", 10000);
        EXPECT_TRUE(connection->connect());
        EXPECT_FALSE(connection->connect());
        EXPECT_TRUE(connection->set_lingering_timeout(0));
    });

    connection_pointer connection = listener->accept();
    EXPECT_TRUE(connection->set_lingering_timeout(0));
    thread.join();
    EXPECT_TRUE(listener->listening());
}

TEST(socket, unconnected_send_receive)
{
    auto connection = connection::create("127.0.0.1", 10000);
    buffer data;
    EXPECT_FALSE(connection->send(""));
    EXPECT_FALSE(connection->receive(data, 0));
}

TEST(socket, terminate_try_to_connect)
{
    auto connection = connection::create("240.0.0.1", 65535);

    std::thread thread([&connection] { EXPECT_FALSE(connection->connect()); });

    usleep(10000);
    connection->close();
    thread.join();
}

TEST(socket, terminate_try_to_accept)
{
    listener_pointer listener = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listener->set_lingering_timeout(0));
    EXPECT_TRUE(listener->listening());

    std::thread thread([&listener] {
        EXPECT_EQ(connection_pointer(), listener->accept());
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
    listener_pointer listener = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listener->set_lingering_timeout(0));
    EXPECT_TRUE(listener->listening());

    std::thread thread([] {
        auto connection = connection::create("127.0.0.1", 10000);
        EXPECT_TRUE(connection->connect());
        EXPECT_NE(connection_pointer(), connection);
        EXPECT_TRUE(connection->set_lingering_timeout(0));
        buffer data;
        EXPECT_TRUE(connection->receive(data, 8));
        EXPECT_EQ(buffer({0, 1, 2, 3}), data);
        data = {4, 5, 6, 7};
        EXPECT_TRUE(connection->send(data));
    });

    connection_pointer connection = listener->accept();
    EXPECT_NE(connection_pointer(), connection);
    EXPECT_TRUE(connection->set_lingering_timeout(0));
    buffer data = {0, 1, 2, 3};
    EXPECT_TRUE(connection->send(data));
    data.clear();
    EXPECT_TRUE(connection->receive(data, 8));
    EXPECT_EQ(buffer({4, 5, 6, 7}), data);

    thread.join();
    EXPECT_TRUE(listener->listening());
}

} // namespace hutzn
