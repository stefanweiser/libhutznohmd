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

#include "_server.hpp"

namespace rest
{

namespace http
{

std::shared_ptr<Server> createServer(
    const std::string & host,
    const uint16_t & port,
    const AccessFn & accessFn)
{
    auto socket = rest::socket::listen(host, port);
    return std::make_shared<Server>(socket, accessFn);
}

Server::Server(
    const rest::socket::ListenerPtr & socket,
    const AccessFn & accessFn)
    : m_socket(socket)
    , m_accessFn(accessFn)
{}

rest::socket::ConnectionPtr Server::accept()
{
    return m_socket->accept();
}

void Server::parseRequest(const rest::socket::ConnectionPtr & connection)
{
    std::vector<uint8_t> data;
    if (false == connection->receive(data, 4000))
    {
        return;
    }
}

} // namespace http

} // namespace rest
