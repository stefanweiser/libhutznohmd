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

#ifndef LIBHUTZNOHMD_COMMUNICATION_INTERNET_SOCKET_LISTENER_HPP
#define LIBHUTZNOHMD_COMMUNICATION_INTERNET_SOCKET_LISTENER_HPP

#include <memory>
#include <string>

#include "libhutznohmd/communication.hpp"

namespace hutzn
{

class internet_socket_listener;

//! @brief Shortcut type to use an @ref internet_socket_listener as reference-
//! counted type.
using internet_socket_listener_ptr = std::shared_ptr<internet_socket_listener>;

//! @brief Implements a listener for internet sockets.
class internet_socket_listener : public listener
{
public:
    //! @brief Creates a new internet socket listener.
    //!
    //! Uses host and port to create the listener. Returns the listener after
    //! resolving and binding to the IP and port.
    //! @param[in] host Host IP to listen on as string.
    //! @param[in] port Port to listen on.
    //! @return         The newly created listener, which has bound to the
    //!                 address and is ready to accept connections.
    static internet_socket_listener_ptr create(const std::string& host,
                                               const uint16_t& port);

    //! @brief Constructs a internet socket listener.
    //!
    //! Used to bind to a socket.
    //! @param[in] socket A socket file descriptor.
    explicit internet_socket_listener(const int32_t& socket);

    //! @brief Safely shuts down the socket.
    //!
    //! First shuts down the socket (if not already done) and closes it
    //! afterwards.
    ~internet_socket_listener(void) noexcept(true) override;

    //! @copydoc listener::accept()
    connection_ptr accept(void) const override;

    //! @copydoc listener::listening()
    bool listening(void) const override;

    //! @copydoc listener::stop()
    void stop(void) override;

    //! @copydoc listener::set_lingering_timeout()
    bool set_lingering_timeout(const int32_t& timeout) override;

private:
    //! Is true, when the object is listening and false otherwise.
    bool is_listening_;

    //! Stores the file descriptor of the listening or closed socket.
    int32_t socket_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_COMMUNICATION_INTERNET_SOCKET_LISTENER_HPP
