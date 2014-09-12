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

#ifndef LIBREST_SOCKET_SOCKET_INTERFACE_HPP
#define LIBREST_SOCKET_SOCKET_INTERFACE_HPP

#include <cstdint>
#include <memory>
#include <vector>

#include <rest.hpp>

namespace rest
{

namespace socket
{

class connection_socket_interface
{
public:
    virtual ~connection_socket_interface();
    virtual bool connect() = 0;
    virtual void close() = 0;
    virtual bool receive(rest::buffer& data, const size_t& max_size) = 0;
    virtual bool send(const rest::buffer& data) = 0;
    virtual bool send(const std::string& data) = 0;
};

typedef std::shared_ptr<connection_socket_interface> connection_pointer;

class listener_socket_interface
{
public:
    virtual ~listener_socket_interface();
    virtual connection_pointer accept() const = 0;
    virtual bool listening() const = 0;
    virtual void stop() = 0;
};

typedef std::shared_ptr<listener_socket_interface> listener_pointer;

connection_pointer connect(const std::string& host, const uint16_t& port);
listener_pointer listen(const std::string& host, const uint16_t& port);

} // namespace socket

} // namespace rest

#endif // LIBREST_SOCKET_SOCKET_INTERFACE_HPP
