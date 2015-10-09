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

#include <http/server.hpp>

#include <libhutznohmd/mock_sockets.hpp>

using namespace testing;

namespace hutzn
{

namespace http
{

TEST(server, parsing_request)
{
    bool called = false;
    auto transaction =
        [&called](const request_interface& /*request*/,
                  response_interface& /*response*/) { called = true; };
    server server(listener_pointer(), transaction);

    auto socket = std::make_shared<connection_interface_mock>();
    EXPECT_CALL(*socket, receive(_, _))
        .WillOnce(Invoke([](buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = buffer(request_data.begin(), request_data.end());
            return true;
        }))
        .WillRepeatedly(Invoke([](buffer& /*data*/, const size_t & /*max_size*/)
                                   -> bool { return false; }));
    EXPECT_CALL(*socket, send(An<const buffer&>())).Times(1).WillRepeatedly(
        Return(true));
    EXPECT_CALL(*socket, send(An<const std::string&>()))
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, called);
    server.parse_request(socket);
    EXPECT_EQ(true, called);
}

} // namespace http

} // namespace hutzn