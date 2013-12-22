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

#include "socket.hpp"

namespace rest
{

namespace http
{

Socket::Socket(const std::string& host,
               const uint16_t& port,
               const Socket::Listen&)
    : m_socket(0)
{
    m_socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if ( m_socket == -1 )
    {
        throw std::bad_alloc();
    }

    sockaddr_in addr;
    addr.sin_addr.s_addr = ::inet_addr(host.c_str());
    addr.sin_port = ::htons(port);
    addr.sin_family = AF_INET;

    if ( ::bind(m_socket, (sockaddr *) &addr, sizeof(addr)) == -1 )
    {
        ::close(m_socket);
        throw std::bad_alloc();
    }

    if ( ::listen(m_socket, 4) == -1 )
    {
        ::close(m_socket);
        throw std::bad_alloc();
    }
}

Socket::Socket(const std::string& host,
               const uint16_t& port,
               const Socket::Connect&)
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

Socket::Socket(const int& socket)
    : m_socket(socket)
{}

Socket::~Socket()
{
    close(m_socket);
}

std::shared_ptr<Socket> Socket::accept()
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int client = ::accept(m_socket, (sockaddr *) &addr, &len);
    if ( client == -1 )
    {
        return std::shared_ptr<Socket>();
    }

    return std::make_shared<Socket>(client);
}

} // namespace http

} // namespace rest
