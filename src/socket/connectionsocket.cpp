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
#include <sys/socket.h>
#include <unistd.h>

#include <socket/utility.hpp>

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
    const int socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket == -1)
    {
        return std::shared_ptr<ConnectionSocket>();
    }

    const ::sockaddr_in addr = fillAddress(host, port);
    std::shared_ptr<ConnectionSocket> result = std::make_shared<ConnectionSocket>(socket, addr);

    return result;
}

ConnectionSocket::ConnectionSocket(const int & socket)
    : m_isConnected(true)
    , m_socket(socket)
    , m_addr()
{}

ConnectionSocket::ConnectionSocket(const int & socket, const ::sockaddr_in & addr)
    : m_isConnected(false)
    , m_socket(socket)
    , m_addr(addr)
{}

ConnectionSocket::~ConnectionSocket()
{
    close();
    closeSignalSafe(m_socket);
}

namespace
{

union Addr
{
    const ::sockaddr * base;
    const ::sockaddr_in * in;
};

}

bool ConnectionSocket::connect()
{
    if (true == m_isConnected)
    {
        return false;
    }
    Addr addr;
    addr.in = &m_addr;
    if (connectSignalSafe(m_socket, addr.base, sizeof(m_addr)) != 0)
    {
        close();
        return false;
    }
    m_isConnected = true;
    return true;
}

void ConnectionSocket::close()
{
    m_isConnected = false;
    ::shutdown(m_socket, SHUT_RDWR);
}

bool ConnectionSocket::receive(rest::Buffer & data, const size_t & maxSize)
{
    if (false == m_isConnected)
    {
        return false;
    }

    const size_t oldSize = data.size();
    data.resize(oldSize + maxSize);
    const ::ssize_t received = recvSignalSafe(m_socket, data.data() + oldSize, maxSize, 0);

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
    if (false == m_isConnected)
    {
        return false;
    }

    ::ssize_t sent = 0;

    do
    {
        const void * q = p + sent;
        const size_t t = s - sent;
        const ::ssize_t sentBlock = sendSignalSafe(m_socket, q, t, 0);

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
