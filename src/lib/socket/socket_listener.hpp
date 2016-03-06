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

#ifndef LIBHUTZNOHMD_SOCKET_SOCKET_LISTENER_HPP
#define LIBHUTZNOHMD_SOCKET_SOCKET_LISTENER_HPP

#include <memory>
#include <string>

#include "libhutznohmd/sockets.hpp"

namespace hutzn
{

//! @copydoc listener_interface
class socket_listener : public listener_interface
{
public:
    //! @brief Creates a new socket listener.
    //!
    //! Uses host and port to create the listener. Returns the listener after
    //! resolving and binding to the IP and port.
    //! @param[in] host Host IP to listen on as string.
    //! @param[in] port Port to listen on.
    //! @return         The newly created listener, which has bound to the
    //!                 address and is ready to accept connections.
    static std::shared_ptr<socket_listener> create(const std::string& host,
                                                   const uint16_t& port);

    //! @brief Constructs a socket listener. Used to bind to a socket.
    //! @param[in] socket A socket file descriptor.
    explicit socket_listener(const int32_t& socket);

    //! @copydoc listener_interface::~listener_interface()
    ~socket_listener(void) noexcept(true) override;

    //! @copydoc listener_interface::accept()
    connection_ptr accept(void) const override;

    //! @copydoc listener_interface::listening()
    bool listening(void) const override;

    //! @copydoc listener_interface::stop()
    void stop(void) override;

    //! @copydoc listener_interface::set_lingering_timeout()
    bool set_lingering_timeout(const int32_t& timeout) override;

private:
    //! Is true, when the object is listening and false otherwise.
    bool is_listening_;

    //! Stores the file descriptor of the listening or closed socket.
    int32_t socket_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_SOCKET_SOCKET_LISTENER_HPP
