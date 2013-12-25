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

namespace http
{

ConnectionSocket::ConnectionSocket(const std::string& host,
                                   const uint16_t& port)
    : m_socket(0)
{
    sockaddr_in addr;
    if ( !::inet_aton(host.c_str(), &addr.sin_addr) )
    {
        hostent *hostname = ::gethostbyname(host.c_str());
        if ( !hostname )
        {
            throw std::bad_alloc();
        }
        addr.sin_addr = *(in_addr*) hostname->h_addr;
    }

    m_socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if ( m_socket == -1 )
    {
        throw std::bad_alloc();
    }

    addr.sin_port = ::htons(port);
    addr.sin_family = AF_INET;

    if ( ::connect(m_socket, (sockaddr*) &addr, sizeof(addr)) == -1 )
    {
        ::close(m_socket);
        throw std::bad_alloc();
    }
}

ConnectionSocket::ConnectionSocket(const int& socket)
    : m_socket(socket)
{}

ConnectionSocket::~ConnectionSocket()
{
    close(m_socket);
}

bool ConnectionSocket::receive(std::vector<uint8_t>& /*data*/)
{
    return false;
}

bool ConnectionSocket::send(const std::vector<uint8_t>& /*data*/)
{
    return false;
}

} // namespace http

} // namespace rest
