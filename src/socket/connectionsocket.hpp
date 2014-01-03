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

#ifndef __LIBREST_SOCKET_CONNECTIONSOCKET_HPP__
#define __LIBREST_SOCKET_CONNECTIONSOCKET_HPP__

#include <cstdint>

#include <librest.hpp>

#include <socket/socketinterface.hpp>

namespace rest
{

namespace socket
{

class ConnectionSocket : public ConnectionSocketInterface
{
public:
    explicit ConnectionSocket(const std::string & host, const uint16_t & port);
    explicit ConnectionSocket(const int & socket);
    virtual ~ConnectionSocket();
    virtual bool receive(rest::Buffer & data, const size_t & maxSize);
    virtual bool send(const rest::Buffer & data);

private:
    const int m_socket;
};

} // namespace socket

} // namespace rest

#endif // __LIBREST_SOCKET_CONNECTIONSOCKET_HPP__
