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

#include "connection.hpp"

#include <algorithm>
#include <cassert>
#include <string>

#include "socket/utility.hpp"

namespace hutzn
{

std::shared_ptr<connection> connection::create(const std::string& host,
                                               const uint16_t& port)
{
    std::shared_ptr<connection> result;
    const int32_t socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    // only connect if a valid socket file descriptor was created
    if (socket_fd != -1) {
        const sockaddr_in address = fill_address(host, port);
        // only return a connection if the host and port could be resolved
        // successfully
        if (address.sin_family != AF_UNSPEC) {
            result = std::make_shared<connection>(socket_fd, address);
        }
    }
    return result;
}

connection::connection(const int32_t& socket)
    : is_connected_(true)
    , socket_(socket)
    , address_()
{
}

connection::connection(const int32_t& socket, const sockaddr_in& address)
    : is_connected_(false)
    , socket_(socket)
    , address_(address)
{
}

connection::~connection(void)
{
    // first shutdown and then close the connection before destructing the
    // object
    close();
    const int32_t close_result = close_signal_safe(socket_);
    assert(close_result == 0);
    UNUSED(close_result);
}

void connection::close(void)
{
    is_connected_ = false;
    shutdown(socket_, SHUT_RDWR);
}

bool connection::receive(buffer& data, const size_t& max_size)
{
    bool result = false;
    // receive will only succeed when the socket is connected
    if (is_connected_) {
        const size_t old_size = data.size();
        data.resize(old_size + max_size);
        void* const p = data.data() + old_size;
        // reveive is not called in a loop, because there is propably not more
        // to receive and the user has to decide whether to read more data due
        // to protocol necessities or not
        const ssize_t received = receive_signal_safe(socket_, p, max_size, 0);
        const ssize_t new_extension_size = std::max<ssize_t>(received, 0);
        data.resize(old_size + static_cast<size_t>(new_extension_size));
        result = (received > 0);
    }
    return result;
}

bool connection::send(const buffer& data)
{
    // convert the buffer and use a single send method
    return send(data.data(), data.size());
}

bool connection::send(const std::string& data)
{
    // convert the buffer and use a single send method
    return send(data.data(), data.size());
}

bool connection::send(const char_t* data, const size_t& size)
{
    bool result = false;
    // send will only succeed when the socket is connected
    // also check for a valid size, because signed size_t is used internally
    if (is_connected_ &&
        (size <= static_cast<size_t>(std::numeric_limits<ssize_t>::max()))) {

        const ssize_t ssize = static_cast<ssize_t>(size);
        ssize_t total_sent_size = 0;
        // loop until all is sent
        while (total_sent_size < ssize) {
            const size_t block_size =
                static_cast<size_t>(ssize - total_sent_size);
            const void* const p = data + total_sent_size;
            const ssize_t sent_size =
                send_signal_safe(socket_, p, block_size, 0);
            assert(sent_size <= static_cast<ssize_t>(block_size));

            if (sent_size <= 0) {
                // Exceptional use of extra exit point (breaks MISRA C++:2008
                // Rule 6-6-5). Breaking from the while and double testing the
                // number of totally sent bytes would be a slower and less
                // readable alternative.
                return false;
            }
            total_sent_size += sent_size;
            assert(total_sent_size <= ssize);
        }
        assert(total_sent_size == ssize);

        result = true;
    }
    return result;
}

bool connection::set_lingering_timeout(const int32_t& timeout)
{
    linger lex{1, timeout};
    return setsockopt(socket_, SOL_SOCKET, SO_LINGER, &lex, sizeof(lex)) == 0;
}

bool connection::connect(void)
{
    bool result = false;
    // connecting makes only sense if the socket is not connected
    if (!is_connected_) {

        // This is an accepted exceptional use of an union (breaks MISRA
        // C++:2008 Rule 9-5-1). Alternatively a reinterpret_cast could be used,
        // but anyway there must be a way to fulfill the BSD socket interface.
        union
        {
            sockaddr base;
            sockaddr_in in;
        } addr;

        addr.in = address_;

        if (connect_signal_safe(socket_, &addr.base, sizeof(addr.in)) == 0) {
            is_connected_ = true;
            result = true;
        } else {
            // we'll shutdown the socket in case of a failed try to connect
            close();
        }
    }
    return result;
}

} // namespace hutzn
