/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The libhutznohmd project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The libhutznohmd project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the libhutznohmd project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rest
{

//! Universal data buffer type of the library.
typedef std::vector<char> buffer;

namespace socket
{

class connection_interface
{
public:
    virtual ~connection_interface();
    virtual bool connect() = 0;
    virtual void close() = 0;
    virtual bool receive(rest::buffer& data, const size_t& max_size) = 0;
    virtual bool send(const rest::buffer& data) = 0;
    virtual bool send(const std::string& data) = 0;
    virtual bool set_lingering_timeout(const int& timeout) = 0;
    virtual int socket() const = 0;
};

typedef std::shared_ptr<connection_interface> connection_pointer;

class listener_interface
{
public:
    virtual ~listener_interface();
    virtual connection_pointer accept() const = 0;
    virtual bool listening() const = 0;
    virtual void stop() = 0;
    virtual bool set_lingering_timeout(const int& timeout) = 0;
    virtual int socket() const = 0;
};

typedef std::shared_ptr<listener_interface> listener_pointer;

connection_pointer connect(const std::string& host, const uint16_t& port);
listener_pointer listen(const std::string& host, const uint16_t& port);

} // namespace socket

} // namespace rest

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP
