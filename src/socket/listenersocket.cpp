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

#include <socket/connectionsocket.hpp>

#include "listenersocket.hpp"

namespace rest
{

namespace socket
{

ListenerSocket::ListenerSocket(const std::string& host, const uint16_t& port)
    : m_socket(-1)
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

ListenerSocket::~ListenerSocket()
{
    close(m_socket);
}

std::shared_ptr<ConnectionSocketInterface> ListenerSocket::accept() const
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int client = ::accept(m_socket, (sockaddr *) &addr, &len);
    if ( client == -1 )
    {
        return std::shared_ptr<ConnectionSocketInterface>();
    }

    return std::make_shared<ConnectionSocket>(client);
}

} // namespace socket

} // namespace rest
