/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#include <thread>

#include <gtest/gtest.h>

#include "socket/utility.hpp"

namespace hutzn
{

TEST(socket_utility, close_illegal_socket)
{
    EXPECT_EQ(close_signal_safe(42), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST(socket_utility, accept_illegal_socket)
{
    sockaddr addr;
    socklen_t size = sizeof(addr);
    EXPECT_EQ(accept_signal_safe(42, &addr, &size), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST(socket_utility, connect_illegal_socket)
{
    sockaddr addr;
    EXPECT_EQ(connect_signal_safe(42, &addr, sizeof(addr)), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST(socket_utility, send_illegal_socket)
{
    EXPECT_EQ(send_signal_safe(42, nullptr, 0, 0), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST(socket_utility, receive_illegal_socket)
{
    EXPECT_EQ(receive_signal_safe(42, nullptr, 0, 0), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST(socket_utility, fill_address_ok)
{
    sockaddr_in address = fill_address("127.0.0.1", 0x8000);
    const uint16_t port_result = ntohs(0x8000);
    const uint32_t address_result = ntohl(0x7F000001);
    EXPECT_EQ(AF_INET, address.sin_family);
    EXPECT_EQ(port_result, address.sin_port);
    EXPECT_EQ(address_result, address.sin_addr.s_addr);
}

TEST(socket_utility, fill_address_error)
{
    sockaddr_in address = fill_address("127:0:0:1", 0x8000);
    EXPECT_EQ(AF_UNSPEC, address.sin_family);
}

TEST(socket_utility, fill_address_no_dns)
{
    sockaddr_in address = fill_address("example.com", 0x8000);
    EXPECT_EQ(AF_UNSPEC, address.sin_family);
}

} // namespace hutzn
