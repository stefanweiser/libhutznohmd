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

#include <thread>

#include <gtest/gtest.h>

#define private public
#define protected public

#include <socket/utility.hpp>

namespace rest
{

TEST(socket_utility, fill_address_ok)
{
    ::sockaddr_in address = socket::fill_address("127.0.0.1", 0x8000);
    EXPECT_EQ(address.sin_family, AF_INET);
    EXPECT_EQ(address.sin_port, ntohs(0x8000));
    EXPECT_EQ(address.sin_addr.s_addr, ntohl(0x7F000001));
}

TEST(socket_utility, fill_address_error)
{
    ::sockaddr_in address = socket::fill_address("127:0:0:1", 0x8000);
    EXPECT_EQ(address.sin_family, AF_UNSPEC);
}

} // namespace rest
