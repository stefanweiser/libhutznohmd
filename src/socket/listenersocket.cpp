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

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <socket/connectionsocket.hpp>

#include "listenersocket.hpp"

namespace rest
{

namespace socket
{

listener_pointer listen(const std::string & host, const uint16_t & port)
{
    return listener_socket::create(host, port);
}

namespace
{

union address_union {
    ::sockaddr base;
    ::sockaddr_in in;
};

}

std::shared_ptr<listener_socket> listener_socket::create(const std::string & host,
        const uint16_t & port)
{
    const int socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket == -1) {
        return std::shared_ptr<listener_socket>();
    }

    std::shared_ptr<listener_socket> result = std::make_shared<listener_socket>(socket);

    address_union address;
    address.in = fill_address(host, port);
    int result1 = ::bind(result->socket_, &(address.base), sizeof(address));
    int result2 = ::listen(result->socket_, 4);
    if ((result1 == -1) || (result2 == -1)) {
        return std::shared_ptr<listener_socket>();
    }

    return result;
}

listener_socket::listener_socket(const int & socket)
    : is_listening_(true)
    , socket_(socket)
{}

listener_socket::~listener_socket()
{
    stop();
    close_signal_safe(socket_);
}

connection_pointer listener_socket::accept()
{
    address_union address;
    ::socklen_t size = sizeof(address);

    const int client = accept_signal_safe(socket_, &(address.base), &size);
    if (client == -1) {
        return connection_pointer();
    }

    return std::make_shared<connection_socket>(client);
}

bool listener_socket::listening() const
{
    return is_listening_;
}

void listener_socket::stop()
{
    is_listening_ = false;
    ::shutdown(socket_, SHUT_RDWR);
}

} // namespace socket

} // namespace rest
