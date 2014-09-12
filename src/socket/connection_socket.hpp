/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREST_SOCKET_CONNECTION_SOCKET_HPP
#define LIBREST_SOCKET_CONNECTION_SOCKET_HPP

#include <cstdint>

#include <netinet/in.h>

#include <rest.hpp>

#include <socket/socket_interface.hpp>

namespace rest
{

namespace socket
{

class connection_socket : public connection_socket_interface
{
public:
    static std::shared_ptr<connection_socket> create(const std::string& host,
                                                     const uint16_t& port);

    explicit connection_socket(const int& s);
    explicit connection_socket(const int& s, const ::sockaddr_in& address);
    virtual ~connection_socket();
    virtual bool connect();
    virtual void close();
    virtual bool receive(rest::buffer& data, const size_t& max_size);
    virtual bool send(const rest::buffer& data);
    virtual bool send(const std::string& data);

    int socket() const;

private:
    bool send(const char* buffer, const size_t& size);

    bool is_connected_;
    int socket_;
    const ::sockaddr_in address_;
};

} // namespace socket

} // namespace rest

#endif // LIBREST_SOCKET_CONNECTION_SOCKET_HPP
