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

#define private public
#define protected public

#include <http/server.hpp>

TEST(HttpServer, Construction)
{
    rest::HttpServer httpServer("127.0.0.1",
                                10000,
                                rest::http::AcceptFn(),
                                rest::http::AccessFn());

    EXPECT_NE(httpServer.m_pDaemon, nullptr);
}

TEST(HttpServer, AcceptNoCallback)
{
    rest::HttpServer httpServer("127.0.0.1",
                                10000,
                                rest::http::AcceptFn(),
                                rest::http::AccessFn());

    EXPECT_EQ(httpServer.accept(nullptr, 0), true);
}

TEST(HttpServer, AcceptCallback)
{
    bool result   = true;
    auto acceptFn = [&result](const sockaddr *, const socklen_t) -> bool
                    {
                        return result;
                    };
    rest::HttpServer httpServer("127.0.0.1",
                                10000,
                                acceptFn,
                                rest::http::AccessFn());

    EXPECT_EQ(httpServer.accept(nullptr, 0), true);
    result = false;
    EXPECT_EQ(httpServer.accept(nullptr, 0), false);
}

TEST(HttpServer, Completed)
{
    rest::HttpServer httpServer("127.0.0.1",
                                10000,
                                rest::http::AcceptFn(),
                                rest::http::AccessFn());
    std::string * tmp = new std::string();

    httpServer.completed(nullptr,
                         reinterpret_cast<void **>(&tmp),
                         MHD_REQUEST_TERMINATED_COMPLETED_OK);
    EXPECT_EQ(tmp, nullptr);

    if ( tmp != nullptr )
    {
        delete tmp;
    }
}
