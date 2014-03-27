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

#ifndef __LIBREST_SOCKET_SOCKETINTERFACE_HPP__
#define __LIBREST_SOCKET_SOCKETINTERFACE_HPP__

#include <cstdint>
#include <memory>
#include <vector>

#include <librest.hpp>

namespace rest
{

namespace socket
{

class connection_socket_interface
{
public:
    virtual ~connection_socket_interface() {}
    virtual bool connect() = 0;
    virtual void close() = 0;
    virtual bool receive(rest::buffer & data, const size_t & max_size) = 0;
    virtual bool send(const rest::buffer & data) = 0;
    virtual bool send(const std::string & data) = 0;
};

typedef std::shared_ptr<connection_socket_interface> connection_pointer;

class listener_socket_interface
{
public:
    virtual ~listener_socket_interface() {}
    virtual connection_pointer accept() = 0;
    virtual bool listening() const = 0;
    virtual void stop() = 0;
};

typedef std::shared_ptr<listener_socket_interface> listener_pointer;

connection_pointer connect(const std::string & host, const uint16_t & port);
listener_pointer listen(const std::string & host, const uint16_t & port);

} // namespace socket

} // namespace rest

#endif // __LIBREST_SOCKET_SOCKETINTERFACE_HPP__
