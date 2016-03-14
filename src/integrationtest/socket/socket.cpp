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

#include <thread>

#include <gtest/gtest.h>

#include "socket/socket_connection.hpp"

namespace hutzn
{

TEST(socket, listener_construction)
{
    auto listnr = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listnr->set_lingering_timeout(0));
    EXPECT_NE(listener_ptr(), listnr);
    EXPECT_TRUE(listnr->listening());
}

TEST(socket, wrong_construction_arguments)
{
    EXPECT_EQ(listener_ptr(), listen("127.0.0:1", 10000));
}

TEST(socket, reuse_host_and_port)
{
    auto listnr = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listnr->set_lingering_timeout(0));
    EXPECT_EQ(listener_ptr(), listen("127.0.0.1", 10000));
    EXPECT_TRUE(listnr->listening());
}

TEST(socket, accepting_closed_socket)
{
    auto listnr = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listnr->set_lingering_timeout(0));
    listnr->stop();
    EXPECT_EQ(connection_ptr(), listnr->accept());
}

TEST(socket, connecting_closed_socket)
{
    auto conn = socket_connection::create("127.0.0.1", 10000);
    conn->close();
    EXPECT_FALSE(conn->connect());
}

TEST(socket, connection_refused)
{
    auto conn = socket_connection::create("127.0.0.1", 10000);
    EXPECT_FALSE(conn->connect());
}

TEST(socket, receive_send_closed_socket)
{
    auto listnr = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listnr->set_lingering_timeout(0));
    EXPECT_TRUE(listnr->listening());

    bool connected = false;
    bool disconnected = false;
    std::thread thread([&disconnected, &connected] {
        auto conn = socket_connection::create("127.0.0.1", 10000);
        EXPECT_TRUE(conn->connect());
        EXPECT_TRUE(conn->set_lingering_timeout(0));
        connected = true;
        while (disconnected == false) {
            usleep(1);
        }

        buffer data;
        EXPECT_FALSE(conn->receive(data, 8));

        conn->close();
        EXPECT_FALSE(conn->receive(data, 8));
        EXPECT_FALSE(conn->send(data));
    });

    connection_ptr connection = listnr->accept();
    EXPECT_TRUE(connection->set_lingering_timeout(0));
    while (connected == false) {
        usleep(1);
    }
    connection.reset();
    disconnected = true;
    thread.join();
    EXPECT_TRUE(listnr->listening());
}

TEST(socket, double_connect)
{
    listener_ptr listnr = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listnr->set_lingering_timeout(0));
    EXPECT_TRUE(listnr->listening());

    std::thread thread([] {
        auto conn = socket_connection::create("127.0.0.1", 10000);
        EXPECT_TRUE(conn->connect());
        EXPECT_FALSE(conn->connect());
        EXPECT_TRUE(conn->set_lingering_timeout(0));
    });

    connection_ptr connection = listnr->accept();
    EXPECT_TRUE(connection->set_lingering_timeout(0));
    thread.join();
    EXPECT_TRUE(listnr->listening());
}

TEST(socket, unconnected_send_receive)
{
    auto conn = socket_connection::create("127.0.0.1", 10000);
    buffer data;
    EXPECT_FALSE(conn->send(""));
    EXPECT_FALSE(conn->receive(data, 0));
}

TEST(socket, terminate_try_to_connect)
{
    auto conn = socket_connection::create("240.0.0.1", 65535);

    std::thread thread([&conn] { EXPECT_FALSE(conn->connect()); });

    usleep(10000);
    conn->close();
    thread.join();
}

TEST(socket, terminate_try_to_accept)
{
    listener_ptr listnr = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listnr->set_lingering_timeout(0));
    EXPECT_TRUE(listnr->listening());

    std::thread thread([&listnr] {
        EXPECT_EQ(connection_ptr(), listnr->accept());
        EXPECT_FALSE(listnr->listening());
    });

    usleep(10000);
    EXPECT_TRUE(listnr->listening());
    listnr->stop();
    thread.join();
    EXPECT_FALSE(listnr->listening());
}

TEST(socket, normal_use_case)
{
    listener_ptr listnr = listen("127.0.0.1", 10000);
    EXPECT_TRUE(listnr->set_lingering_timeout(0));
    EXPECT_TRUE(listnr->listening());

    std::thread thread([] {
        auto conn = socket_connection::create("127.0.0.1", 10000);
        EXPECT_TRUE(conn->connect());
        EXPECT_NE(connection_ptr(), conn);
        EXPECT_TRUE(conn->set_lingering_timeout(0));
        buffer data;
        EXPECT_TRUE(conn->receive(data, 8));
        EXPECT_EQ(buffer({0, 1, 2, 3}), data);
        data = {4, 5, 6, 7};
        EXPECT_TRUE(conn->send(data));
    });

    connection_ptr connection = listnr->accept();
    EXPECT_NE(connection_ptr(), connection);
    EXPECT_TRUE(connection->set_lingering_timeout(0));
    buffer data = {0, 1, 2, 3};
    EXPECT_TRUE(connection->send(data));
    data.clear();
    EXPECT_TRUE(connection->receive(data, 8));
    EXPECT_EQ(buffer({4, 5, 6, 7}), data);

    thread.join();
    EXPECT_TRUE(listnr->listening());
}

} // namespace hutzn
