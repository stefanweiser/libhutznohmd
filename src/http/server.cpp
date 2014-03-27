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

server_pointer create_server(const std::string & host,
                             const uint16_t & port,
                             const transaction_function & transaction_functor)
{
    auto socket = rest::socket::listen(host, port);
    return std::make_shared<server>(socket, transaction_functor);
}

server::server(const rest::socket::listener_pointer & socket,
               const transaction_function & transaction_functor)
    : threads_()
    , socket_(socket)
    , transaction_functor_(transaction_functor)
    , shutdown_(false)
{}

void server::run()
{
    while (false == shutdown_) {
        auto connection = socket_->accept();
        if (connection) {
            threads_.insert(std::make_shared<std::thread>(&server::parse_request,
                            this,
                            connection));
        }
    }

    for (auto thread : threads_) {
        thread->join();
    }
    threads_.clear();
}

void server::stop()
{
    shutdown_ = true;
    socket_->stop();
}

void server::parse_request(const rest::socket::connection_pointer & connection)
{
    request request(connection);
    request.parse();

    response response(connection);
    if (transaction_functor_) {
        transaction_functor_(request, response);
    }
    response.deliver();
}

} // namespace http

} // namespace rest
