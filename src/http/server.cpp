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

#include <http/request.hpp>
#include <http/response.hpp>

#include "server.hpp"

namespace rest
{

namespace http
{

ServerPtr createServer(
    const std::string & host,
    const uint16_t & port,
    const TransactionFn & transactionFn)
{
    auto socket = rest::socket::listen(host, port);
    return std::make_shared<Server>(socket, transactionFn);
}

Server::Server(
    const rest::socket::ListenerPtr & socket,
    const TransactionFn & transactionFn)
    : m_threads()
    , m_socket(socket)
    , m_transactionFn(transactionFn)
    , m_shutdown(false)
{}

void Server::run()
{
    while (false == m_shutdown)
    {
        auto connection = m_socket->accept();
        if (connection)
        {
            m_threads.insert(std::make_shared<std::thread>(&Server::parseRequest,
                             this,
                             connection));
        }
    }

    for (auto thread : m_threads)
    {
        thread->join();
    }
    m_threads.clear();
}

void Server::stop()
{
    m_shutdown = true;
    m_socket->stop();
}

void Server::parseRequest(const rest::socket::ConnectionPtr & connection)
{
    Request request(connection);
    request.parse();

    Response response(connection);
    if (m_transactionFn)
    {
        m_transactionFn(request, response);
    }
    response.deliver();
}

} // namespace http

} // namespace rest
