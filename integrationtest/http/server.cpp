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

    usleep(1000);

    EXPECT_NE(httpServer.m_pDaemon, nullptr);
}

TEST(HttpServer, AcceptNoCallback)
{
    rest::HttpServer httpServer("127.0.0.1",
                                10000,
                                rest::http::AcceptFn(),
                                rest::http::AccessFn());

    usleep(1000);

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

    usleep(1000);

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

    usleep(1000);

    httpServer.completed(nullptr,
                         reinterpret_cast<void **>(&tmp),
                         MHD_REQUEST_TERMINATED_COMPLETED_OK);
    EXPECT_EQ(tmp, nullptr);
}

TEST(HttpServer, Access)
{
    std::string accessedData;
    rest::HttpServer httpServer("127.0.0.1",
                                10000,
                                rest::http::AcceptFn(),
                                rest::http::AccessFn());

    std::string* tmp = nullptr;
    size_t size = 0;
    int answer = httpServer.access(nullptr,
                                   "/",
                                   MHD_HTTP_METHOD_GET,
                                   MHD_HTTP_VERSION_1_1,
                                   nullptr,
                                   &size,
                                   reinterpret_cast<void **>(&tmp));
    EXPECT_EQ(answer, MHD_YES);
    EXPECT_NE(tmp, nullptr);

    std::string data = "abcdef";
    size = data.size();
    answer = httpServer.access(nullptr,
                               "/",
                               MHD_HTTP_METHOD_GET,
                               MHD_HTTP_VERSION_1_1,
                               data.c_str(),
                               &size,
                               reinterpret_cast<void **>(&tmp));
    EXPECT_EQ(answer, MHD_YES);
    EXPECT_EQ(size, 0);
    EXPECT_EQ(*tmp, data);

    answer = httpServer.access(nullptr,
                               "/",
                               MHD_HTTP_METHOD_GET,
                               MHD_HTTP_VERSION_1_1,
                               nullptr,
                               &size,
                               reinterpret_cast<void **>(&tmp));
    EXPECT_EQ(answer, MHD_NO);

    usleep(1000);

    if ( tmp != nullptr )
    {
        delete tmp;
    }
}

TEST(HttpServer, CopyAndAssignment)
{
    rest::HttpServer httpServer("127.0.0.1",
                                10000,
                                rest::http::AcceptFn(),
                                rest::http::AccessFn());

    usleep(1000);

    rest::HttpServer httpServer2(httpServer);
    EXPECT_EQ(httpServer2.m_pDaemon, nullptr);
}
