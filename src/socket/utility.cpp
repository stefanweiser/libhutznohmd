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

#include <array>

#include <assert.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <unistd.h>

#include <socket/connectionsocket.hpp>

#include "utility.hpp"

namespace rest
{

namespace socket
{

void closeSignalSafe(int fd)
{
    int res;
    do
    {
        res = ::close(fd);
    }
    while ((res == -1) && (errno == EINTR));
}

int acceptSignalSafe(int sockfd, ::sockaddr * addr, socklen_t * len)
{
    int res;
    do
    {
        res = ::accept(sockfd, addr, len);
    }
    while ((res == -1) && (errno == EINTR));
    return res;
}

int connectSignalSafe(int sockfd, const ::sockaddr * addr, socklen_t len)
{
    int res = ::connect(sockfd, addr, len);
    if (res == -1)
    {

        if (errno != EINTR)
        {
            return res;
        }
        pollfd p {sockfd, POLLOUT, 0};
        do
        {
            res = ::poll(&p, 1, -1);
            if ((res == -1) && (errno != EINTR))
            {
                return res;
            }
        }
        while (res == -1);

        int error;
        socklen_t l = sizeof(error);
        res = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &l);
        if (res == -1)
        {
            return res;
        }
        if (error != 0)
        {
            return -1;
        }
    }
    return res;
}

ssize_t sendSignalSafe(int fd, const void * buf, size_t n, int flags)
{
    ssize_t sent;
    do
    {
        sent = ::send(fd, buf, n, flags);
    }
    while ((sent == -1) && (errno == EINTR));
    return sent;
}

ssize_t recvSignalSafe(int fd, void * buf, size_t n, int flags)
{
    ssize_t received;
    do
    {
        received = ::recv(fd, buf, n, flags);
    }
    while ((received == -1) && (errno == EINTR));
    return received;
}

::sockaddr_in fillAddress(const std::string & host, const uint16_t & port)
{
    ::sockaddr_in addr;

    if (!::inet_aton(host.c_str(), &addr.sin_addr))
    {
        const ::hostent * hostname = ::gethostbyname(host.c_str());

        if (!hostname)
        {
            addr.sin_family = AF_UNSPEC;
            return addr;
        }
        addr.sin_addr = * (::in_addr *) hostname->h_addr;
    }

    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    return addr;
}

} // namespace socket

} // namespace rest
