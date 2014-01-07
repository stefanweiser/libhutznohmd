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

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <socket/connectionsocket.hpp>

#include "listenersocket.hpp"

namespace rest
{

namespace socket
{

ListenerPtr listen(const std::string & host, const uint16_t & port)
{
    return std::make_shared<ListenerSocket>(host, port);
}

ListenerSocket::ListenerSocket(const std::string & host, const uint16_t & port)
    : m_socket(::socket(PF_INET, SOCK_STREAM, 0))
    , m_notifier()
{
    if (m_socket == -1)
    {
        throw std::bad_alloc();
    }

    ::sockaddr_in addr = fillAddress(host, port);
    int res1 = ::bind(m_socket, (::sockaddr *) &addr, sizeof(addr));
    int res2 = ::listen(m_socket, 4);
    if ((res1 == -1) || (res2 == -1))
    {
        ::close(m_socket);
        throw std::bad_alloc();
    }
}

ListenerSocket::~ListenerSocket()
{
    ::shutdown(m_socket, SHUT_RDWR);
    ::close(m_socket);
}

ConnectionPtr ListenerSocket::accept() const
{
    ::sockaddr_in addr;
    ::socklen_t len = sizeof(addr);

    ::fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(m_socket, &readSet);
    FD_SET(m_notifier.receiver(), &readSet);
    int maxFd = std::max(m_socket, m_notifier.receiver());
    if (::select(maxFd + 1, &readSet, nullptr, nullptr, nullptr) <= 0)
    {
        return ConnectionPtr();
    }

    if (FD_ISSET(m_notifier.receiver(), &readSet) != 0)
    {
        return ConnectionPtr();
    }

    const int client = ::accept(m_socket, (::sockaddr *) &addr, &len);

    if (client == -1)
    {
        return ConnectionPtr();
    }

    return std::make_shared<ConnectionSocket>(client);
}

void ListenerSocket::stop()
{
    m_notifier.notify();
}

} // namespace socket

} // namespace rest
