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

#include <cstdint>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "connectionsocket.hpp"

namespace rest
{

namespace socket
{

ConnectionPtr connect(
    const std::string & host,
    const uint16_t & port)
{
    return ConnectionSocket::create(host, port);
}

std::shared_ptr<ConnectionSocket> ConnectionSocket::create(
    const std::string & host,
    const uint16_t & port)
{
    int socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket == -1)
    {
        return std::shared_ptr<ConnectionSocket>();
    }

    std::shared_ptr<ConnectionSocket> result;
    result = std::make_shared<ConnectionSocket>(socket);

    ::fd_set readSet;
    ::fd_set writeSet;
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_SET(result->m_socket, &writeSet);
    FD_SET(result->m_notifier.receiver(), &readSet);
    int maxFd = std::max(result->m_socket, result->m_notifier.receiver());
    if (::select(maxFd + 1, &readSet, &writeSet, nullptr, nullptr) <= 0)
    {
        return std::shared_ptr<ConnectionSocket>();
    }

    if (FD_ISSET(result->m_notifier.receiver(), &readSet) != 0)
    {
        return std::shared_ptr<ConnectionSocket>();
    }

    ::sockaddr_in addr = fillAddress(host, port);
    if (::connect(result->m_socket, (::sockaddr *) &addr, sizeof(addr)) == -1)
    {
        return std::shared_ptr<ConnectionSocket>();
    }

    return result;
}

ConnectionSocket::ConnectionSocket(const int & socket)
    : m_socket(socket)
    , m_notifier()
{}

ConnectionSocket::~ConnectionSocket()
{
    ::shutdown(m_socket, SHUT_RDWR);
    ::close(m_socket);
}

bool ConnectionSocket::receive(rest::Buffer & data, const size_t & maxSize)
{
    if (m_socket < 0)
    {
        return false;
    }

    const size_t oldSize = data.size();
    data.resize(oldSize + maxSize);
    const ::ssize_t received = ::recv(m_socket, data.data() + oldSize, maxSize, 0);

    if (received <= 0)
    {
        return false;
    }

    data.resize(oldSize + received);
    return true;
}

bool ConnectionSocket::send(const rest::Buffer & data)
{
    return send(data.data(), data.size());
}

bool ConnectionSocket::send(const std::string & data)
{
    return send(data.data(), data.size());
}

bool ConnectionSocket::send(const char * p, const size_t & s)
{
    if (m_socket < 0)
    {
        return false;
    }

    ::ssize_t sent = 0;

    do
    {
        const void * q = p + sent;
        const size_t t = s - sent;
        const ::ssize_t sentBlock = ::send(m_socket, q, t, 0);

        if (sentBlock <= 0)
        {
            return false;
        }
        sent += sentBlock;
    }
    while (sent < static_cast<::ssize_t>(s));

    return true;
}

} // namespace socket

} // namespace rest
