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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/response.hpp>

#include <socket/mocksocketinterface.hpp>

using namespace testing;

TEST(response, construction_destruction)
{
    auto response = std::make_shared<rest::http::response>(rest::socket::connection_pointer());
    EXPECT_EQ(response->connection_, rest::socket::connection_pointer());
    EXPECT_EQ(response->status_code_, rest::http::status_code::INTERNAL_SERVER_ERROR);
    EXPECT_EQ(response->version_, rest::http::version::HTTP_1_1);
    EXPECT_EQ(response->headers_.size(), 0);
    EXPECT_EQ(response->data_.size(), 0);
}

TEST(response, set_and_deliver)
{
    auto socket = std::make_shared<rest::socket::connection_socket_mock>();
    rest::http::response response(socket);

    response.set_data({ '0' });
    response.set_header("Content-Length", "1");
    response.set_status_code(rest::http::status_code::OK);
    response.set_version(rest::http::version::HTTP_1_1);

    EXPECT_CALL(*socket, send(An<const rest::buffer &>()))
    .Times(1)
    .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string &>()))
    .Times(1)
    .WillRepeatedly(Return(true));
    response.deliver();

    response.set_version(rest::http::version::HTTP_1_0);
    EXPECT_CALL(*socket, send(An<const rest::buffer &>()))
    .Times(1)
    .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string &>()))
    .Times(1)
    .WillRepeatedly(Return(true));
    response.deliver();

    response.set_version(rest::http::version::HTTP_UNKNOWN);
    EXPECT_CALL(*socket, send(An<const rest::buffer &>()))
    .Times(1)
    .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string &>()))
    .Times(1)
    .WillRepeatedly(Return(true));
    response.deliver();
}
