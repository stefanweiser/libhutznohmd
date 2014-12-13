/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#include <algorithm>
#include <cstdint>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <socket/utility.hpp>

#include "connection.hpp"

namespace hutzn
{

namespace socket
{

std::shared_ptr<connection> connection::create(const std::string& host,
                                               const uint16_t& port)
{
    const int socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket == -1) {
        return std::shared_ptr<connection>();
    }

    const ::sockaddr_in address = fill_address(host, port);
    auto result = std::make_shared<connection>(socket, address);

    return result;
}

connection::connection(const int& s)
    : is_connected_(true)
    , socket_(s)
    , address_()
{
}

connection::connection(const int& s, const ::sockaddr_in& address)
    : is_connected_(false)
    , socket_(s)
    , address_(address)
{
}

connection::~connection()
{
    close();
    close_signal_safe(socket_);
}

namespace
{

union address_union
{
    const ::sockaddr* base;
    const ::sockaddr_in* in;
};

} // namespace

void connection::close()
{
    is_connected_ = false;
    ::shutdown(socket_, SHUT_RDWR);
}

bool connection::receive(hutzn::buffer& data, const size_t& max_size)
{
    if (false == is_connected_) {
        return false;
    }

    const size_t old_size = data.size();
    data.resize(old_size + max_size);
    const ::ssize_t received =
        receive_signal_safe(socket_, data.data() + old_size, max_size, 0);

    data.resize(old_size +
                static_cast<size_t>(std::max<::ssize_t>(received, 0)));
    return (received > 0);
}

bool connection::send(const hutzn::buffer& data)
{
    return send(data.data(), data.size());
}

bool connection::send(const std::string& data)
{
    return send(data.data(), data.size());
}

bool connection::send(const char* buffer, const size_t& size)
{
    if (false == is_connected_) {
        return false;
    }

    ::ssize_t total_sent_size = 0;

    do {
        const size_t block_size = size - static_cast<size_t>(total_sent_size);
        const ::ssize_t sent_block_size =
            send_signal_safe(socket_, buffer + total_sent_size, block_size, 0);

        if (sent_block_size <= 0) {
            return false;
        }
        total_sent_size += sent_block_size;
    } while (total_sent_size < static_cast<::ssize_t>(size));

    return true;
}

bool connection::set_lingering_timeout(const int& timeout)
{
    ::linger l = ::linger{1, timeout};
    return ::setsockopt(socket_, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) == 0;
}

int connection::socket() const
{
    return socket_;
}

bool connection::connect()
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

} // namespace socket

} // namespace hutzn
