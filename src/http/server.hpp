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

#ifndef LIBHUTZNOHMD_HTTP_SERVER_HPP
#define LIBHUTZNOHMD_HTTP_SERVER_HPP

#include <functional>
#include <memory>
#include <set>
#include <thread>

#include <socket/listener.hpp>

#include <hutzn.hpp>

namespace hutzn
{

namespace http
{

class server : public server_interface
{
public:
    server(const listener_pointer& s,
           const transaction_function& transaction_functor);

    void run() override;
    void stop() override;

    void parse_request(const connection_pointer& connection);

    const listener_pointer& socket() const;

private:
    std::set<std::shared_ptr<std::thread>> threads_;
    listener_pointer socket_;
    transaction_function transaction_functor_;
    bool shutdown_;
};

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HTTP_SERVER_HPP
