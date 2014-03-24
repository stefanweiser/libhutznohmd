/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

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
    return ListenerSocket::create(host, port);
}

namespace
{

union Addr {
    ::sockaddr base;
    ::sockaddr_in in;
};

}

std::shared_ptr<ListenerSocket> ListenerSocket::create(
    const std::string & host,
    const uint16_t & port)
{
    const int socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket == -1) {
        return std::shared_ptr<ListenerSocket>();
    }

    std::shared_ptr<ListenerSocket> result;
    result = std::make_shared<ListenerSocket>(socket);

    Addr addr;
    addr.in = fillAddress(host, port);
    int res1 = ::bind(result->m_socket, &(addr.base), sizeof(addr));
    int res2 = ::listen(result->m_socket, 4);
    if ((res1 == -1) || (res2 == -1)) {
        return std::shared_ptr<ListenerSocket>();
    }

    return result;
}

ListenerSocket::ListenerSocket(const int & socket)
    : m_isListening(true)
    , m_socket(socket)
{}

ListenerSocket::~ListenerSocket()
{
    stop();
    closeSignalSafe(m_socket);
}

ConnectionPtr ListenerSocket::accept()
{
    Addr addr;
    ::socklen_t len = sizeof(addr);

    const int client = acceptSignalSafe(m_socket, &(addr.base), &len);
    if (client == -1) {
        return ConnectionPtr();
    }

    return std::make_shared<ConnectionSocket>(client);
}

bool ListenerSocket::listening() const
{
    return m_isListening;
}

void ListenerSocket::stop()
{
    m_isListening = false;
    ::shutdown(m_socket, SHUT_RDWR);
}

} // namespace socket

} // namespace rest
