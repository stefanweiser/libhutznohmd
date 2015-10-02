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

#include <http/request.hpp>
#include <http/response.hpp>

#include "server.hpp"

namespace hutzn
{

namespace http
{

server_pointer create_server(const std::string& host, const uint16_t& port,
                             const transaction_function& access_functor)
{
    auto socket = listen(host, port);
    return std::make_shared<server>(socket, access_functor);
}

server::server(const listener_pointer& s,
               const transaction_function& transaction_functor)
    : threads_()
    , socket_(s)
    , transaction_functor_(transaction_functor)
    , shutdown_(false)
{
}

void server::run()
{
    while (!shutdown_) {
        auto connection = socket_->accept();
        if (connection) {
            threads_.insert(std::make_shared<std::thread>(
                &server::parse_request, this, connection));
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

void server::parse_request(const connection_pointer& connection)
{
    request req(connection, request::parameters{true});
    req.parse();

    response resp(connection);
    if (transaction_functor_) {
        transaction_functor_(req, resp);
    }
    resp.deliver();

    // Close the connection if needed.
    if (!req.keeps_connection()) {
        connection->close();
    }
}

const listener_pointer& server::socket() const
{
    return socket_;
}

} // namespace http

} // namespace hutzn
