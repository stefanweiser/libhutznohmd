/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#include "internet_socket_listener.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>

#include "communication/internet_socket_connection.hpp"
#include "communication/utility.hpp"

namespace hutzn
{

listener_ptr listen(const std::string& host, const uint16_t& port)
{
    return internet_socket_listener::create(host, port);
}

internet_socket_listener_ptr internet_socket_listener::create(
    const std::string& host, const uint16_t& port)
{
    internet_socket_listener_ptr result;
    const int32_t socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    // only listen if a valid socket file descriptor was created
    if (socket_fd >= 0) {

        // This is an accepted exceptional use of an union (breaks MISRA
        // C++:2008 Rule 9-5-1). Alternatively a reinterpret_cast could be used,
        // but anyway there must be a way to fulfill the BSD socket interface.
        union {
            sockaddr base;
            sockaddr_in in;
        } addr;

        addr.in = fill_address(host, port);
        if (addr.in.sin_family != AF_UNSPEC) {
            const int32_t result1 =
                bind(socket_fd, &addr.base, sizeof(addr.in));
            const int32_t result2 = ::listen(socket_fd, 4);
            // return a valid object only if bind and listen does not return an
            // error
            if ((result1 != -1) && (result2 != -1)) {
                result = std::make_shared<internet_socket_listener>(socket_fd);
            }
        }
    }

    return result;
}

internet_socket_listener::internet_socket_listener(const int32_t& socket)
    : is_listening_(true)
    , socket_(socket)
{
}

internet_socket_listener::~internet_socket_listener(void) noexcept(true)
{
    // stop listening and close the socket before destructing the object
    stop();
    const int32_t close_result = close_signal_safe(socket_);
    assert(close_result == 0);
    UNUSED(close_result);
}

connection_ptr internet_socket_listener::accept(void) const
{
    connection_ptr result;

    // accept will only succeed when the socket is connected
    if (is_listening_) {

        // This is an accepted exceptional use of an union (breaks MISRA
        // C++:2008 Rule 9-5-1). Alternatively a reinterpret_cast could be used,
        // but anyway there must be a way to fulfill the BSD socket interface.
        union {
            sockaddr base;
            sockaddr_in in;
        } addr;

        socklen_t size = sizeof(addr.in);
        const int32_t client = accept_signal_safe(socket_, &addr.base, &size);
        // return an empty object when accept signalises an error
        if (client >= 0) {
            result = std::make_shared<internet_socket_connection>(client);
        }
    }
    return result;
}

bool internet_socket_listener::listening(void) const
{
    return is_listening_;
}

void internet_socket_listener::stop(void)
{
    is_listening_ = false;
    shutdown(socket_, SHUT_RDWR);
}

bool internet_socket_listener::set_lingering_timeout(const int32_t& timeout)
{
    const linger lex{1, timeout};
    return setsockopt(socket_, SOL_SOCKET, SO_LINGER, &lex, sizeof(lex)) == 0;
}

} // namespace hutzn
