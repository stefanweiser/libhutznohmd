/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREST_SOCKET_UTILITY_HPP
#define LIBREST_SOCKET_UTILITY_HPP

#include <cstdint>
#include <memory>
#include <string>

#include <arpa/inet.h>

#include <rest.hpp>

#include <socket/socket_interface.hpp>

namespace rest
{

namespace socket
{

void close_signal_safe(int file_descriptor);

int accept_signal_safe(int file_descriptor, ::sockaddr * address, socklen_t * size);

int connect_signal_safe(int socket_file_descriptor, const ::sockaddr * address, socklen_t size);

ssize_t send_signal_safe(int file_descriptor, const void * buffer, size_t size, int flags);

ssize_t receive_signal_safe(int file_descriptor, void * buffer, size_t size, int flags);

::sockaddr_in fill_address(const std::string & host, const uint16_t & port);

} // namespace socket

} // namespace rest

#endif // LIBREST_SOCKET_UTILITY_HPP
