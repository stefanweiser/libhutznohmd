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

#ifndef __LIBREST_SOCKET_UTILITY_HPP__
#define __LIBREST_SOCKET_UTILITY_HPP__

#include <cstdint>
#include <memory>
#include <string>

#include <arpa/inet.h>

#include <librest.hpp>

#include <socket/socketinterface.hpp>

namespace rest
{

namespace socket
{

void closeSignalSafe(int fd);

int acceptSignalSafe(int fd, ::sockaddr * addr, socklen_t * len);

int connectSignalSafe(int sockfd, const ::sockaddr * addr, socklen_t len);

ssize_t sendSignalSafe(int fd, const void * buf, size_t n, int flags);

ssize_t recvSignalSafe(int fd, void * buf, size_t n, int flags);

::sockaddr_in fillAddress(const std::string & host, const uint16_t & port);

} // namespace socket

} // namespace rest

#endif // __LIBREST_SOCKET_UTILITY_HPP__
