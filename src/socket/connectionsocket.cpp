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

ConnectionSocket::ConnectionSocket(const std::string & host,
                                   const uint16_t & port)
    : m_socket(-1)
{
    m_socket = ::socket(PF_INET, SOCK_STREAM, 0);

    if (m_socket == -1)
    {
        throw std::bad_alloc();
    }

    sockaddr_in addr;

    if (!::inet_aton(host.c_str(), &addr.sin_addr))
    {
        hostent * hostname = ::gethostbyname(host.c_str());

        if (!hostname)
        {
            throw std::bad_alloc();
        }
        addr.sin_addr = * (in_addr *) hostname->h_addr;
    }

    addr.sin_port = ::htons(port);
    addr.sin_family = AF_INET;

    if (::connect(m_socket, (sockaddr *) &addr, sizeof(addr)) == -1)
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
    close(m_socket);
}

bool ConnectionSocket::receive(
    std::vector<uint8_t> & data,
    const size_t & maxSize)
{
    if (m_socket < 0)
    {
        return false;
    }

    const size_t oldSize = data.size();
    data.resize(oldSize + maxSize);
    ssize_t received = ::recv(m_socket, data.data() + oldSize, data.size(), 0);

    if (received <= 0)
    {
        return false;
    }

    data.resize(oldSize + received);
    return true;
}

bool ConnectionSocket::send(const std::vector<uint8_t> & data)
{
    if (m_socket < 0)
    {
        return false;
    }

    ssize_t sent = 0;

    do
    {
        ssize_t sentBlock = ::send(m_socket,
                                   data.data() + sent,
                                   data.size() - sent,
                                   0);

        if (sentBlock < 0)
        {
            return false;
        }
        sent += sentBlock;
    }
    while (sent < ssize_t (data.size()));

    return true;
}

} // namespace socket

} // namespace rest
