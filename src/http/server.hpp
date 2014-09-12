/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREST_HTTP_SERVER_HPP
#define LIBREST_HTTP_SERVER_HPP

#include <functional>
#include <memory>
#include <set>
#include <thread>

#include <socket/listener_socket.hpp>

#include <rest.hpp>

namespace rest
{

namespace http
{

class server: public server_interface
{
public:
    server(const rest::socket::listener_pointer & s,
           const transaction_function & transaction_functor);

    virtual void run();
    virtual void stop();

    void parse_request(const rest::socket::connection_pointer & connection);

    const rest::socket::listener_pointer & socket() const;

private:
    std::set<std::shared_ptr<std::thread>> threads_;
    rest::socket::listener_pointer socket_;
    transaction_function transaction_functor_;
    bool shutdown_;
};

} // namespace http

} // namespace rest

#endif // LIBREST_HTTP_SERVER_HPP
