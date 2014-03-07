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

#include <socket/utility.hpp>

TEST(SocketUtility, FillAddressOk)
{
    ::sockaddr_in addr = rest::socket::fillAddress("127.0.0.1", 0x8000);
    EXPECT_EQ(addr.sin_family, AF_INET);
    EXPECT_EQ(addr.sin_port, ntohs(0x8000));
    EXPECT_EQ(addr.sin_addr.s_addr, ntohl(0x7F000001));
}

TEST(SocketUtility, FillAddressThrow)
{
    ::sockaddr_in addr = rest::socket::fillAddress("127:0:0:1", 0x8000);
    EXPECT_EQ(addr.sin_family, AF_UNSPEC);
}
