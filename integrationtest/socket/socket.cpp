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

#include <thread>

#include <gtest/gtest.h>

#define private public
#define protected public

#include <socket/connectionsocket.hpp>
#include <socket/listenersocket.hpp>

int initRandomizer()
{
    srand(time(NULL));
    return 0;
}

uint16_t dicePort()
{
    return 10000 + (rand() % 50000);
}

static int once = initRandomizer();

TEST(Socket, ConstructionNoThrow)
{
    EXPECT_NO_THROW(rest::socket::ListenerSocket("127.0.0.1", dicePort()));
}

TEST(Socket, AcceptSendReceive)
{
    uint16_t port = dicePort();
    rest::socket::ListenerSocket socket("127.0.0.1", port);
    EXPECT_NE(socket.m_socket, -1);

    std::thread t([&port]
    {
        rest::socket::ConnectionSocket socket2("localhost", port);
        EXPECT_NE(socket2.m_socket, -1);

        rest::Buffer data = { 0, 1, 2, 3, 4, 5, 6, 7 };
        EXPECT_EQ(socket2.send(data), true);
        data.clear();
        EXPECT_EQ(socket2.receive(data, 8), true);
        EXPECT_EQ(data.size(), 4);
        EXPECT_EQ(data, rest::Buffer({ 0, 1, 2, 3 }));
    });

    rest::socket::ConnectionPtr connection;
    connection = socket.accept();
    rest::socket::ConnectionPtr empty;
    EXPECT_NE(connection, empty);
    rest::socket::ConnectionSocket * c;
    c = dynamic_cast<rest::socket::ConnectionSocket *>(connection.get());
    EXPECT_NE(c->m_socket, -1);

    rest::Buffer data = { 0, 1, 2, 3 };
    EXPECT_EQ(connection->send(data), true);
    data.clear();
    EXPECT_EQ(connection->receive(data, 16), true);
    EXPECT_EQ(data.size(), 8);
    EXPECT_EQ(data, rest::Buffer({ 0, 1, 2, 3, 4, 5, 6, 7 }));

    t.join();
}

TEST(Socket, WrongConstructionArguments)
{
    uint16_t port = dicePort();
    rest::socket::ListenerPtr listener;
    EXPECT_NO_THROW(listener = rest::socket::listen("localhost", port));

    EXPECT_THROW(rest::socket::ListenerSocket socket("127.0.0.1", port), std::bad_alloc);
}

TEST(Socket, AcceptingClosedSocket)
{
    rest::socket::ListenerSocket listener("127.0.0.1", dicePort());
    EXPECT_EQ(::close(listener.m_socket), 0);
    EXPECT_EQ(listener.accept(), rest::socket::ConnectionPtr());
}

TEST(Socket, ConnectionRefused)
{
    EXPECT_THROW(rest::socket::ConnectionSocket connection("127.0.0.1", dicePort()), std::bad_alloc);
}

TEST(Socket, ReceiveSendClosedSocket)
{
    uint16_t port = dicePort();
    rest::socket::ListenerPtr listener;
    EXPECT_NO_THROW(listener = rest::socket::listen("localhost", port));

    bool disconnected = false;
    std::thread t([&disconnected, &port]
    {
        rest::socket::ConnectionSocket connection2("localhost", port);
        while (disconnected == false)
        {
            usleep(1);
        }

        rest::Buffer data;
        EXPECT_FALSE(connection2.receive(data, 8));

        EXPECT_EQ(::close(connection2.m_socket), 0);
        EXPECT_FALSE(connection2.receive(data, 8));
        EXPECT_FALSE(connection2.send(data));

        const_cast<int &>(connection2.m_socket) = -1;
        EXPECT_FALSE(connection2.receive(data, 8));
        EXPECT_FALSE(connection2.send(data));
    });

    rest::socket::ConnectionPtr connection = listener->accept();
    connection.reset();
    disconnected = true;
    t.join();
}

TEST(Socket, NormalUseCase)
{
    uint16_t port = dicePort();
    rest::socket::ListenerPtr listener;
    EXPECT_NO_THROW(listener = rest::socket::listen("localhost", port));

    std::thread t([&port]
    {
        rest::socket::ConnectionPtr connection;
        EXPECT_NO_THROW(connection = rest::socket::connect("localhost", port));
        EXPECT_NE(connection, rest::socket::ConnectionPtr());
        rest::Buffer data;
        EXPECT_TRUE(connection->receive(data, 8));
        EXPECT_EQ(data, rest::Buffer({ 0, 1, 2, 3 }));
        data = { 4, 5, 6, 7 };
        EXPECT_TRUE(connection->send(data));
    });

    rest::socket::ConnectionPtr connection = listener->accept();
    EXPECT_NE(connection, rest::socket::ConnectionPtr());
    rest::Buffer data = { 0, 1, 2, 3 };
    EXPECT_TRUE(connection->send(data));
    data.clear();
    EXPECT_TRUE(connection->receive(data, 8));
    EXPECT_EQ(data, rest::Buffer({ 4, 5, 6, 7 }));

    t.join();
}
