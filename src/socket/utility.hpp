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

#ifndef LIBHUTZNOHMD_SOCKET_UTILITY_HPP
#define LIBHUTZNOHMD_SOCKET_UTILITY_HPP

#include <cstdint>
#include <string>

#include <arpa/inet.h>

namespace hutzn
{

//! @brief Calls the API function close and handles interfering signals.
//!
//! The function will return when the file descriptor is closed.
void close_signal_safe(const int file_descriptor) noexcept(true);

//! @brief Calls the API function accept and handles interfering signals.
//!
//! The function will return when there is a connection in the queue. It returns
//! the file descriptor of the accepted connection.
int accept_signal_safe(const int file_descriptor, sockaddr* const address,
                       socklen_t* const size);

//! @brief Calls the API function connect and handles interfering signals.
//!
//! This means that the function will return when a connection is established.
//! The connection is established, when the socket gets writable.
int connect_signal_safe(const int socket_file_descriptor,
                        const sockaddr* const address, const socklen_t size);

//! @brief Calls the API function send and handles interfering signals.
//!
//! It returns the number of sent bytes. When the socket is getting closed while
//! sending data, it will return -1.
ssize_t send_signal_safe(const int file_descriptor, const void* const buffer,
                         const size_t size, const int flags) noexcept(true);

//! @brief Calls the API function recv and handles interfering signals.
//!
//! It returns the number of received bytes. When the socket is getting closed
//! while receiving data, it will return 0.
ssize_t receive_signal_safe(const int file_descriptor, void* const buffer,
                            const size_t size, const int flags) noexcept(true);

//! @brief Converts a host string and a port into a sockaddr_in struct.
//!
//! This is needed when communicating with other API functions of the network
//! stack. The function does not support domain name lookup.
sockaddr_in fill_address(const std::string& host, const uint16_t& port);

} // namespace hutzn

#endif // LIBHUTZNOHMD_SOCKET_UTILITY_HPP
