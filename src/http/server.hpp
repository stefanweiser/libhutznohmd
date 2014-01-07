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

#ifndef __LIBREST_HTTP_SERVER_HPP__
#define __LIBREST_HTTP_SERVER_HPP__

#include <functional>
#include <memory>
#include <set>
#include <thread>

#include <socket/listenersocket.hpp>

#include <librest.hpp>

namespace rest
{

namespace http
{

class Server: public ServerInterface
{
public:
    Server(
        const rest::socket::ListenerPtr & socket,
        const TransactionFn & transactionFn);

    virtual ~Server();

    virtual void run();
    virtual void stop();

private:
    void parseRequest(const rest::socket::ConnectionPtr & connection);

    std::set<std::shared_ptr<std::thread>> m_threads;
    rest::socket::ListenerPtr m_socket;
    TransactionFn m_transactionFn;
    bool m_shutdown;
};

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_SERVER_HPP__
