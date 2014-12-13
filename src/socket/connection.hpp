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

#ifndef LIBHUTZNOHMD_SOCKET_CONNECTION_HPP
#define LIBHUTZNOHMD_SOCKET_CONNECTION_HPP

#include <cstdint>

#include <netinet/in.h>

#include <libhutznohmd/sockets.hpp>

namespace hutzn
{

namespace socket
{

class connection : public connection_interface
{
public:
    static std::shared_ptr<connection> create(const std::string& host,
                                              const uint16_t& port);

    explicit connection(const int& s);
    explicit connection(const int& s, const ::sockaddr_in& address);
    virtual ~connection() override;
    virtual void close() override;
    virtual bool receive(hutzn::buffer& data, const size_t& max_size) override;
    virtual bool send(const hutzn::buffer& data) override;
    virtual bool send(const std::string& data) override;
    virtual bool set_lingering_timeout(const int& timeout) override;
    virtual int socket() const override;

    bool connect();

private:
    bool send(const char* buffer, const size_t& size);

    bool is_connected_;
    int socket_;
    const ::sockaddr_in address_;
};

} // namespace socket

} // namespace hutzn

#endif // LIBHUTZNOHMD_SOCKET_CONNECTION_HPP
