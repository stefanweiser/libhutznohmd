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

#include <thread>

#include <gtest/gtest.h>

#define private public
#define protected public

#include <socket/connectionsocket.hpp>
#include <socket/listenersocket.hpp>

namespace
{

void disableTimeWait(int socket)
{
    ::linger l = ::linger {1, 0};
    ::setsockopt(socket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
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

TEST(Socket, ConstructionNoThrow)
{
    auto l = rest::socket::ListenerSocket::create("127.0.0.1", 10000);
    disableTimeWait(l->m_socket);
    EXPECT_NE(l, std::shared_ptr<rest::socket::ListenerSocket>());
    EXPECT_TRUE(l->listening());
}

TEST(Socket, WrongConstructionArguments)
{
    rest::socket::ListenerPtr listener;
    listener = rest::socket::listen("localhost", 10000);
    disableTimeWait(getSocket(listener));

    EXPECT_EQ(rest::socket::listen("127.0.0.1", 10000), rest::socket::ListenerPtr());
    EXPECT_TRUE(listener->listening());
}

TEST(Socket, AcceptingClosedSocket)
{
    auto listener = rest::socket::ListenerSocket::create("127.0.0.1", 10000);
    disableTimeWait(listener->m_socket);
    EXPECT_EQ(::close(listener->m_socket), 0);
    EXPECT_EQ(listener->accept(), rest::socket::ConnectionPtr());
}

TEST(Socket, ConnectingShutDownSocket)
{
    rest::socket::ConnectionPtr connection;
    connection = rest::socket::ConnectionSocket::create("127.0.0.1", 10000);
    connection->close();
    EXPECT_FALSE(connection->connect());
}

TEST(Socket, ConnectionRefused)
{
    rest::socket::ConnectionPtr connection;
    connection = rest::socket::ConnectionSocket::create("127.0.0.1", 10000);
    EXPECT_FALSE(connection->connect());
}

TEST(Socket, ReceiveSendClosedSocket)
{
    rest::socket::ListenerPtr listener;
    listener = rest::socket::listen("localhost", 10000);
    disableTimeWait(getSocket(listener));
    EXPECT_TRUE(listener->listening());

    bool connected = false;
    bool disconnected = false;
    std::thread t([&disconnected, &connected]
    {
        std::shared_ptr<rest::socket::ConnectionSocket> connection;
        connection = rest::socket::ConnectionSocket::create("localhost", 10000);
        EXPECT_TRUE(connection->connect());
        disableTimeWait(connection->m_socket);
        connected = true;
        while (disconnected == false)
        {
            usleep(1);
        }

        rest::Buffer data;
        EXPECT_FALSE(connection->receive(data, 8));

        EXPECT_EQ(::close(connection->m_socket), 0);
        EXPECT_FALSE(connection->receive(data, 8));
        EXPECT_FALSE(connection->send(data));

        const_cast<int &>(connection->m_socket) = -1;
        EXPECT_FALSE(connection->receive(data, 8));
        EXPECT_FALSE(connection->send(data));
    });

    rest::socket::ConnectionPtr connection = listener->accept();
    disableTimeWait(getSocket(connection));
    while (connected == false)
    {
        usleep(1);
    }
    connection.reset();
    disconnected = true;
    t.join();
    EXPECT_TRUE(listener->listening());
}

TEST(Socket, DoubleConnect)
{
    rest::socket::ListenerPtr listener;
    listener = rest::socket::listen("localhost", 10000);
    disableTimeWait(getSocket(listener));
    EXPECT_TRUE(listener->listening());

    std::thread t([]
    {
        std::shared_ptr<rest::socket::ConnectionSocket> connection;
        connection = rest::socket::ConnectionSocket::create("localhost", 10000);
        EXPECT_TRUE(connection->connect());
        EXPECT_FALSE(connection->connect());
        disableTimeWait(connection->m_socket);
    });

    rest::socket::ConnectionPtr connection = listener->accept();
    disableTimeWait(getSocket(connection));
    t.join();
    EXPECT_TRUE(listener->listening());
}

TEST(Socket, UnconnectedSendReceive)
{
    std::shared_ptr<rest::socket::ConnectionSocket> connection;
    connection = rest::socket::ConnectionSocket::create("localhost", 10000);
    rest::Buffer data;
    EXPECT_FALSE(connection->send(""));
    EXPECT_FALSE(connection->receive(data, 0));
}

TEST(Socket, TerminateTryToConnect)
{
    std::shared_ptr<rest::socket::ConnectionSocket> connection;
    connection = rest::socket::ConnectionSocket::create("240.0.0.1", 65535);

    std::thread t([&connection]
    {
        EXPECT_FALSE(connection->connect());
    });

    usleep(10000);
    connection->close();
    t.join();
}

TEST(Socket, TerminateTryToAccept)
{
    rest::socket::ListenerPtr listener;
    listener = rest::socket::listen("localhost", 10000);
    disableTimeWait(getSocket(listener));
    EXPECT_TRUE(listener->listening());

    std::thread t([&listener]
    {
        EXPECT_EQ(listener->accept(), rest::socket::ConnectionPtr());
        EXPECT_FALSE(listener->listening());
    });

    usleep(10000);
    EXPECT_TRUE(listener->listening());
    listener->stop();
    t.join();
    EXPECT_FALSE(listener->listening());
}

TEST(Socket, NormalUseCase)
{
    rest::socket::ListenerPtr listener;
    listener = rest::socket::listen("localhost", 10000);
    disableTimeWait(getSocket(listener));
    EXPECT_TRUE(listener->listening());

    std::thread t([]
    {
        rest::socket::ConnectionPtr connection;
        connection = rest::socket::connect("localhost", 10000);
        EXPECT_TRUE(connection->connect());
        EXPECT_NE(connection, rest::socket::ConnectionPtr());
        disableTimeWait(getSocket(connection));
        rest::Buffer data;
        EXPECT_TRUE(connection->receive(data, 8));
        EXPECT_EQ(data, rest::Buffer({ 0, 1, 2, 3 }));
        data = { 4, 5, 6, 7 };
        EXPECT_TRUE(connection->send(data));
    });

    rest::socket::ConnectionPtr connection = listener->accept();
    EXPECT_NE(connection, rest::socket::ConnectionPtr());
    disableTimeWait(getSocket(connection));
    rest::Buffer data = { 0, 1, 2, 3 };
    EXPECT_TRUE(connection->send(data));
    data.clear();
    EXPECT_TRUE(connection->receive(data, 8));
    EXPECT_EQ(data, rest::Buffer({ 4, 5, 6, 7 }));

    t.join();
    EXPECT_TRUE(listener->listening());
}
