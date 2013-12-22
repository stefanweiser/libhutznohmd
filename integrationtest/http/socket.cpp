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

#include <http/socket.hpp>

TEST(Socket, ConstructionNoThrow)
{
    EXPECT_NO_THROW(rest::http::Socket("127.0.0.1",
                                       10000,
                                       rest::http::listen));
}

TEST(Socket, Accept)
{
    rest::http::Socket socket("127.0.0.1",
                              10000,
                              rest::http::listen);
    EXPECT_NE(socket.m_socket, -1);

    std::thread t([]
    {
        rest::http::Socket socket2("localhost",
                                   10000,
                                   rest::http::connect);
        EXPECT_NE(socket2.m_socket, -1);
    });

    std::shared_ptr<rest::http::Socket> connection = socket.accept();
    EXPECT_NE(connection, std::shared_ptr<rest::http::Socket>());
    EXPECT_NE(connection->m_socket, -1);

    t.join();
}
