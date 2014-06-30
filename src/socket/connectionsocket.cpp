/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdint>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <socket/utility.hpp>

#include "connectionsocket.hpp"

namespace rest
{

namespace socket
{

connection_pointer connect(const std::string & host,
                           const uint16_t & port)
{
    return connection_socket::create(host, port);
}

std::shared_ptr<connection_socket> connection_socket::create(const std::string & host,
        const uint16_t & port)
{
    const int socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket == -1) {
        return std::shared_ptr<connection_socket>();
    }

    const ::sockaddr_in address = fill_address(host, port);
    auto result = std::make_shared<connection_socket>(socket, address);

    return result;
}

connection_socket::connection_socket(const int & s)
    : is_connected_(true)
    , socket_(s)
    , address_()
{}

connection_socket::connection_socket(const int & s, const ::sockaddr_in & address)
    : is_connected_(false)
    , socket_(s)
    , address_(address)
{}

connection_socket::~connection_socket()
{
    close();
    close_signal_safe(socket_);
}

namespace
{

union address_union {
    const ::sockaddr * base;
    const ::sockaddr_in * in;
};

}

bool connection_socket::connect()
{
    if (true == is_connected_) {
        return false;
    }
    address_union address;
    address.in = &address_;
    if (connect_signal_safe(socket_, address.base, sizeof(address_)) != 0) {
        close();
        return false;
    }
    is_connected_ = true;
    return true;
}

void connection_socket::close()
{
    is_connected_ = false;
    ::shutdown(socket_, SHUT_RDWR);
}

bool connection_socket::receive(rest::buffer & data, const size_t & max_size)
{
    if (false == is_connected_) {
        return false;
    }

    const size_t old_size = data.size();
    data.resize(old_size + max_size);
    const ::ssize_t received = receive_signal_safe(socket_, data.data() + old_size, max_size, 0);

    if (received <= 0) {
        return false;
    }

    data.resize(old_size + received);
    return true;
}

bool connection_socket::send(const rest::buffer & data)
{
    return send(data.data(), data.size());
}

bool connection_socket::send(const std::string & data)
{
    return send(data.data(), data.size());
}

bool connection_socket::send(const char * buffer, const size_t & size)
{
    if (false == is_connected_) {
        return false;
    }

    ::ssize_t sent_size = 0;

    do {
        const void * block = buffer + sent_size;
        const size_t block_size = size - sent_size;
        const ::ssize_t sent_block_size = send_signal_safe(socket_, block, block_size, 0);

        if (sent_block_size <= 0) {
            return false;
        }
        sent_size += sent_block_size;
    } while (sent_size < static_cast<::ssize_t>(size));

    return true;
}

int connection_socket::socket() const
{
    return socket_;
}

} // namespace socket

} // namespace rest
