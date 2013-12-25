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

#include <http/connectionsocket.hpp>
#include <http/listenersocket.hpp>

TEST(Socket, ConstructionNoThrow)
{
    EXPECT_NO_THROW(rest::http::ListenerSocket("127.0.0.1",
                                               10000));
}

TEST(Socket, Accept)
{
    rest::http::ListenerSocket socket("127.0.0.1",
                                      10000);
    EXPECT_NE(socket.m_socket, -1);

    std::thread t([]
    {
        rest::http::ConnectionSocket socket2("localhost",
                                             10000);
        EXPECT_NE(socket2.m_socket, -1);
    });

    std::shared_ptr<rest::http::ConnectionSocketInterface> connection;
    connection = socket.accept();
    std::shared_ptr<rest::http::ConnectionSocketInterface> empty;
    EXPECT_NE(connection, empty);
    rest::http::ConnectionSocket* pConnection;
    pConnection = dynamic_cast<rest::http::ConnectionSocket*>(connection.get());
    EXPECT_NE(pConnection->m_socket, -1);

    t.join();
}
