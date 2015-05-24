/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

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

#include <netinet/in.h>

#include <libhutznohmd/sockets.hpp>

namespace hutzn
{

class connection : public connection_interface
{
public:
    static std::shared_ptr<connection> create(const std::string& host,
                                              const uint16_t& port);

    explicit connection(const int32_t& socket);
    explicit connection(const int32_t& socket, const sockaddr_in& address);
    ~connection(void) override;
    void close(void) override;
    bool receive(buffer& data, const size_t& max_size) override;
    bool send(const buffer& data) override;
    bool send(const std::string& data) override;
    bool set_lingering_timeout(const int32_t& timeout) override;

    bool connect(void);

private:
    bool send(const char_t* buffer, const size_t& size);

    bool is_connected_;
    int32_t socket_;
    const sockaddr_in address_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_SOCKET_CONNECTION_HPP
