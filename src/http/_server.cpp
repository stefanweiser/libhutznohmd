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

Server::Server(const std::string & host,
               const uint16_t & port,
               const AccessFn & accessFn)
    : m_socket(rest::socket::listen(host, port))
    , m_accessFn(accessFn)
    , m_threads()
{}

Server::~Server()
{
    for (std::shared_ptr<std::thread> & ptrThread : m_threads)
    {
        ptrThread->join();
    }
}

void Server::run()
{
    while (true)
    {
        Connection connection;
        connection = m_socket->accept();

        auto thread = std::make_shared<std::thread> (std::bind(&Server::request,
                      this,
                      connection));
        m_threads.push_back(thread);
    }
}

void Server::request(const Connection & /*connection*/)
{}

} // namespace http

} // namespace rest
