/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_COMMUNICATION_INTERNET_SOCKET_CONNECTION_HPP
#define LIBHUTZNOHMD_COMMUNICATION_INTERNET_SOCKET_CONNECTION_HPP

#include <netinet/in.h>

#include "libhutznohmd/communication.hpp"

namespace hutzn
{

class internet_socket_connection;

//! @brief Shortcut type to use an @ref internet_socket_connection as reference-
//! counted type.
using internet_socket_connection_ptr =
    std::shared_ptr<internet_socket_connection>;

//! @brief Implements a connection for internet sockets.
class internet_socket_connection : public connection
{
public:
    //! @brief Creates a new internet socket based connection.
    //!
    //! Uses host and port to create the connection. Returns the connection
    //! after resolving the host and port.
    //! @param[in] host Host IP to connect to as string.
    //! @param[in] port Port on the host to connect to.
    //! @return         The newly created socket connection, which is not yet
    //!                 connected.
    static internet_socket_connection_ptr create(const std::string& host,
                                                 const uint16_t& port);

    //! @brief Constructs a internet socket based connection.
    //!
    //! Used to accept a connection as a server.
    //! @param[in] socket A socket file descriptor.
    explicit internet_socket_connection(const int32_t& socket);

    //! @brief Constructs a internet socket based connection. Used to connect as
    //! a client to a server.
    //! @param[in] socket  A socket file descriptor.
    //! @param[in] address Address of the host.
    explicit internet_socket_connection(const int32_t& socket,
                                        const sockaddr_in& address);

    //! @copydoc connection::~connection()
    ~internet_socket_connection(void) noexcept(true) override;

    //! @copydoc connection::close()
    void close(void) override;

    //! @copydoc block_device::receive()
    bool receive(buffer& data, const size_t& max_size) override;

    //! @copydoc block_device::send()
    bool send(const buffer& data) override;

    //! @copydoc block_device::send()
    bool send(const std::string& data) override;

    //! @copydoc connection::set_lingering_timeout()
    bool set_lingering_timeout(const int32_t& timeout) override;

    //! Connects to the server and returns true, when the connection was
    //! established successfully.
    bool connect(void);

private:
    //! @brief Sends a buffer.
    //!
    //! Internal send method, which is used by all external visible send
    //! methods.
    //! @param[in] buffer Points to the data to send.
    //! @param[in] size   Number of bytes to read.
    //! @return           True when the buffer could be send completely and
    //!                   false otherwise.
    bool send(const char_t* buffer, const size_t& size);

    //! Is true, when the connection is established and false otherwise.
    bool is_connected_;

    //! Stores the file descriptor of the open or closed socket.
    int32_t socket_;

    //! Stores the socket's address with which computer the connection is or was
    //! established.
    const sockaddr_in address_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_COMMUNICATION_INTERNET_SOCKET_CONNECTION_HPP
