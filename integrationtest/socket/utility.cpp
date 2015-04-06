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

#include <thread>

#include <gtest/gtest.h>

#include <socket/utility.hpp>

namespace hutzn
{

TEST(socket_utility, fill_address_ok)
{
    sockaddr_in address = socket::fill_address("127.0.0.1", 0x8000);
    EXPECT_EQ(address.sin_family, AF_INET);
    EXPECT_EQ(address.sin_port, ntohs(0x8000));
    EXPECT_EQ(address.sin_addr.s_addr, ntohl(0x7F000001));
}

TEST(socket_utility, fill_address_error)
{
    sockaddr_in address = socket::fill_address("127:0:0:1", 0x8000);
    EXPECT_EQ(address.sin_family, AF_UNSPEC);
}

TEST(socket_utility, fill_address_no_dns)
{
    sockaddr_in address = socket::fill_address("example.com", 0x8000);
    EXPECT_EQ(address.sin_family, AF_UNSPEC);
}

} // namespace hutzn
