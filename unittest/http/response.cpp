/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <http/response.hpp>

#include <socket/mock_socket_interface.hpp>

using namespace testing;

namespace rest
{

namespace http
{

TEST(response, set_and_deliver)
{
    auto socket = std::make_shared<rest::socket::connection_socket_mock>();
    response response(socket);

    response.set_data({'0'});
    response.set_header("Content-Length", "1");
    response.set_status_code(status_code::OK);
    response.set_version(version::HTTP_1_1);

    EXPECT_CALL(*socket, send(An<const rest::buffer&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    response.deliver();

    response.set_version(version::HTTP_1_0);
    EXPECT_CALL(*socket, send(An<const rest::buffer&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    response.deliver();

    response.set_version(version::HTTP_UNKNOWN);
    EXPECT_CALL(*socket, send(An<const rest::buffer&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    response.deliver();
}

} // namespace http

} // namespace rest
