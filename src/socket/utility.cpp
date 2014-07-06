/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <array>
#include <cassert>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <unistd.h>

#include <socket/connection_socket.hpp>

#include "utility.hpp"

namespace rest
{

namespace socket
{

void close_signal_safe(int file_descriptor)
{
    int result;
    do {
        result = ::close(file_descriptor);
    } while ((result == -1) && (errno == EINTR));
}

int accept_signal_safe(int socket_descriptor, ::sockaddr * address, socklen_t * size)
{
    int result;
    do {
        result = ::accept(socket_descriptor, address, size);
    } while ((result == -1) && (errno == EINTR));
    return result;
}

int connect_signal_safe(int socket_file_descriptor, const ::sockaddr * address, socklen_t size)
{
    int result = ::connect(socket_file_descriptor, address, size);
    if (result == -1) {

        if (errno != EINTR) {
            return result;
        }
        pollfd p {socket_file_descriptor, POLLOUT, 0};
        do {
            result = ::poll(&p, 1, -1);
            if ((result == -1) && (errno != EINTR)) {
                return result;
            }
        } while (result == -1);

        int error;
        socklen_t s = sizeof(error);
        result = getsockopt(socket_file_descriptor, SOL_SOCKET, SO_ERROR, &error, &s);
        if (result == -1) {
            return result;
        }
        if (error != 0) {
            return -1;
        }
    }
    return result;
}

ssize_t send_signal_safe(int file_descriptor, const void * buffer, size_t size, int flags)
{
    ssize_t sent;
    do {
        sent = ::send(file_descriptor, buffer, size, flags);
    } while ((sent == -1) && (errno == EINTR));
    return sent;
}

ssize_t receive_signal_safe(int file_descriptor, void * buffer, size_t size, int flags)
{
    ssize_t received;
    do {
        received = ::recv(file_descriptor, buffer, size, flags);
    } while ((received == -1) && (errno == EINTR));
    return received;
}

::sockaddr_in fill_address(const std::string & host, const uint16_t & port)
{
    ::sockaddr_in address;

    if (!::inet_aton(host.c_str(), &address.sin_addr)) {
        const ::hostent * hostname = ::gethostbyname(host.c_str());

        if (!hostname) {
            address.sin_family = AF_UNSPEC;
            return address;
        }
        address.sin_addr = * (::in_addr *) hostname->h_addr;
    }

    address.sin_port = htons(port);
    address.sin_family = AF_INET;

    return address;
}

} // namespace socket

} // namespace rest
