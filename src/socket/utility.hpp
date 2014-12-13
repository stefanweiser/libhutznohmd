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

#ifndef LIBHUTZNOHMD_SOCKET_UTILITY_HPP
#define LIBHUTZNOHMD_SOCKET_UTILITY_HPP

#include <cstdint>
#include <string>

#include <arpa/inet.h>

namespace hutzn
{

namespace socket
{

//! Calls the API function close and handles interfering signals. This means,
//! that the function will return when the file descriptor is closed.
void close_signal_safe(const int file_descriptor);

//! Calls the API function accept and handles interfering signals. This means,
//! that the function will return when there is a connection in the queue. It
//! returns the file descriptor of the accepted connection.
int accept_signal_safe(const int file_descriptor, sockaddr* address,
                       socklen_t* size);

//! Calls the API function connect and handles interfering signals. This means
//! that the function will return when a connection is established. The
//! connection is established, when the socket gets writable.
int connect_signal_safe(const int socket_file_descriptor,
                        const sockaddr* address, const socklen_t size);

//! Calls the API function send and handles interfering signals. It returns
//! the number of sent bytes. When the socket is getting closed while sending
//! data, it will return -1.
ssize_t send_signal_safe(const int file_descriptor, const void* buffer,
                         const size_t size, const int flags);

//! Calls the API function recv and handles interfering signals. It returns
//! the number of received bytes. When the socket is getting closed while
//! receiving data, it will return -1.
ssize_t receive_signal_safe(const int file_descriptor, void* buffer,
                            const size_t size, const int flags);

//! Converts a host string and a port into a sockaddr_in struct, that is needed
//! when communicating with other API functions of the network stack.
sockaddr_in fill_address(const std::string& host, const uint16_t& port);

} // namespace socket

} // namespace hutzn

#endif // LIBHUTZNOHMD_SOCKET_UTILITY_HPP
