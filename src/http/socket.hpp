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

#ifndef __LIBREST_HTTP_SOCKET_HPP__
#define __LIBREST_HTTP_SOCKET_HPP__

#include <librest.hpp>

namespace rest
{

namespace http
{

class Socket
{
public:
    struct Listen {};
    struct Connect {};

    explicit Socket(const std::string& host,
                    const uint16_t& port,
                    const Listen&);

    explicit Socket(const std::string& host,
                    const uint16_t& port,
                    const Connect&);

    explicit Socket(const int& socket);

    ~Socket();

    std::shared_ptr<Socket> accept();

private:
    int m_socket;
};

static const Socket::Listen listen;
static const Socket::Connect connect;

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_SOCKET_HPP__
