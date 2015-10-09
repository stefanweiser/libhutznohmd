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

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <unistd.h>

#include <system_error>

#include "utility.hpp"

namespace hutzn
{

int32_t close_signal_safe(const int32_t file_descriptor) noexcept(true)
{
    int32_t result;
    do {
        result = close(file_descriptor);
    } while ((result == -1) && (errno == EINTR));
    return result;
}

int32_t accept_signal_safe(const int32_t socket_descriptor,
                           sockaddr* const address,
                           socklen_t* const size) noexcept(true)
{
    int32_t result;
    do {
        result = accept(socket_descriptor, address, size);
    } while ((result == -1) && (errno == EINTR));
    return result;
}

int32_t connect_signal_safe(const int32_t socket_fd,
                            const sockaddr* const address,
                            const socklen_t size) noexcept(true)
{
    // Try to connect.
    int32_t result = connect(socket_fd, address, size);

    // If this fails, try to recover from this error state.
    if ((result == -1) && (errno == EINTR)) {

        // Wait till the socket gets writable.
        pollfd p{socket_fd, POLLOUT, 0};
        do {
            result = poll(&p, 1, -1);
        } while ((result == -1) && (errno == EINTR));

        if (result != -1) {
            // Check the error option of the socket.
            int32_t error;
            socklen_t s = sizeof(error);
            result = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &s);
            if (error != 0) {
                result = -1;
            }
        }
    }
    return result;
}

ssize_t send_signal_safe(const int32_t file_descriptor,
                         const void* const buffer, const size_t size,
                         const int32_t flags) noexcept(true)
{
    ssize_t sent;
    do {
        sent = send(file_descriptor, buffer, size, flags);
    } while ((sent == -1) && (errno == EINTR));
    return sent;
}

ssize_t receive_signal_safe(const int32_t file_descriptor, void* const buffer,
                            const size_t size,
                            const int32_t flags) noexcept(true)
{
    ssize_t received;
    do {
        received = recv(file_descriptor, buffer, size, flags);
    } while ((received == -1) && (errno == EINTR));

    if (received == -1) {
        throw std::system_error(errno, std::system_category());
    }
    return received;
}

sockaddr_in fill_address(const std::string& host,
                         const uint16_t& port) noexcept(true)
{
    sockaddr_in address;
    if (0 == inet_aton(host.c_str(), &address.sin_addr)) {
        address.sin_family = AF_UNSPEC;
    } else {
        address.sin_port = htons(port);
        address.sin_family = AF_INET;
    }
    return address;
}

} // namespace hutzn
