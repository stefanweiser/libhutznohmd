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

#ifndef __LIBREST_SOCKET_CONNECTIONSOCKET_HPP__
#define __LIBREST_SOCKET_CONNECTIONSOCKET_HPP__

#include <cstdint>

#include <netinet/in.h>

#include <librest.hpp>

#include <socket/socketinterface.hpp>

namespace rest
{

namespace socket
{

class connection_socket : public connection_socket_interface
{
public:
    static std::shared_ptr<connection_socket> create(const std::string & host,
            const uint16_t & port);

    explicit connection_socket(const int & socket);
    explicit connection_socket(const int & socket, const ::sockaddr_in & address);
    virtual ~connection_socket();
    virtual bool connect();
    virtual void close();
    virtual bool receive(rest::buffer & data, const size_t & max_size);
    virtual bool send(const rest::buffer & data);
    virtual bool send(const std::string & data);

private:
    bool send(const char * buffer, const size_t & size);

    bool is_connected_;
    int socket_;
    const ::sockaddr_in address_;
};

} // namespace socket

} // namespace rest

#endif // __LIBREST_SOCKET_CONNECTIONSOCKET_HPP__
