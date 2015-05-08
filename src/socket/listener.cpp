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

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <socket/connection.hpp>
#include <socket/utility.hpp>

#include "listener.hpp"

namespace hutzn
{

listener_pointer listen(const std::string& host, const uint16_t& port)
{
    return listener::create(host, port);
}

std::shared_ptr<listener> listener::create(const std::string& host,
                                           const uint16_t& port)
{
    const int socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket == -1) {
        return std::shared_ptr<listener>();
    }

    std::shared_ptr<listener> result = std::make_shared<listener>(socket);

    union
    {
        sockaddr base;
        sockaddr_in in;
    } s;

    s.in = fill_address(host, port);
    int result1 = bind(result->socket_, &s.base, sizeof(s.in));
    int result2 = ::listen(result->socket_, 4);
    if ((result1 == -1) || (result2 == -1)) {
        return std::shared_ptr<listener>();
    }

    return result;
}

listener::listener(const int& s)
    : is_listening_(true)
    , socket_(s)
{
}

listener::~listener()
{
    stop();
    close_signal_safe(socket_);
}

connection_pointer listener::accept() const
{
    if (false == is_listening_) {
        return connection_pointer();
    }

    union
    {
        sockaddr base;
        sockaddr_in in;
    } s;

    socklen_t size = sizeof(s.in);
    const int client = accept_signal_safe(socket_, &s.base, &size);
    if (client == -1) {
        return connection_pointer();
    }

    return std::make_shared<connection>(client);
}

bool listener::listening() const
{
    return is_listening_;
}

void listener::stop()
{
    is_listening_ = false;
    shutdown(socket_, SHUT_RDWR);
}

bool listener::set_lingering_timeout(const int& timeout)
{
    linger l = linger{1, timeout};
    return setsockopt(socket_, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) == 0;
}

} // namespace hutzn
