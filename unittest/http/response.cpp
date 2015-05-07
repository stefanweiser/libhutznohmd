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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <http/response.hpp>

#include <libhutznohmd/mock_sockets.hpp>

using namespace testing;

namespace hutzn
{

namespace http
{

TEST(response, set_and_deliver)
{
    auto socket = std::make_shared<hutzn::connection_interface_mock>();
    response response(socket);

    response.set_data({'0'});
    response.set_header("Content-Length", "1");
    response.set_status_code(hutzn::request::http_status_code::OK);
    response.set_version(version::HTTP_1_1);

    EXPECT_CALL(*socket, send(An<const hutzn::buffer&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    response.deliver();

    response.set_version(version::HTTP_1_0);
    EXPECT_CALL(*socket, send(An<const hutzn::buffer&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    response.deliver();

    response.set_version(version::HTTP_UNKNOWN);
    EXPECT_CALL(*socket, send(An<const hutzn::buffer&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*socket, send(An<const std::string&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    response.deliver();
}

} // namespace http

} // namespace hutzn
