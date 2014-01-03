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

#include <socket/utility.hpp>

#include "connectionsocket.hpp"

namespace rest
{

namespace socket
{

std::shared_ptr<ConnectionSocketInterface> connect(
    const std::string & host,
    const uint16_t & port)
{
    return std::make_shared<ConnectionSocket>(host, port);
}

ConnectionSocket::ConnectionSocket(
    const std::string & host,
    const uint16_t & port)
    : m_socket(::socket(PF_INET, SOCK_STREAM, 0))
{
    if (m_socket == -1)
    {
        throw std::bad_alloc();
    }

    ::sockaddr_in addr = fillAddress(host, port);
    if (::connect(m_socket, (::sockaddr *) &addr, sizeof(addr)) == -1)
    {
        ::close(m_socket);
        throw std::bad_alloc();
    }
}

ConnectionSocket::ConnectionSocket(const int & socket)
    : m_socket(socket)
{}

ConnectionSocket::~ConnectionSocket()
{
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
    if (m_socket < 0)
    {
        return false;
    }

    ::ssize_t sent = 0;

    do
    {
        const void * p = data.data() + sent;
        const size_t s = data.size() - sent;
        const ::ssize_t sentBlock = ::send(m_socket, p, s, 0);

        if (sentBlock <= 0)
        {
            return false;
        }
        sent += sentBlock;
    }
    while (sent < ::ssize_t (data.size()));

    return true;
}

} // namespace socket

} // namespace rest
