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

#ifndef LIBHUTZNOHMD_SOCKET_CONNECTION_HPP
#define LIBHUTZNOHMD_SOCKET_CONNECTION_HPP

#include <netinet/in.h>

#include "libhutznohmd/sockets.hpp"

namespace hutzn
{

//! @copydoc connection_interface
class connection : public connection_interface
{
public:
    //! Creates a new connection by host and port and returns it.
    static std::shared_ptr<connection> create(const std::string& host,
                                              const uint16_t& port);

    //! Constructs a connection. Used to accept a connection as a server.
    explicit connection(const int32_t& socket);

    //! Constructs a connection. Used to connect as a client to a server.
    explicit connection(const int32_t& socket, const sockaddr_in& address);

    //! @copydoc connection_interface::~connection_interface()
    ~connection(void) override;

    //! @copydoc connection_interface::close()
    void close(void) override;

    //! @copydoc block_device_interface::receive()
    bool receive(buffer& data, const size_t& max_size) override;

    //! @copydoc block_device_interface::send()
    bool send(const buffer& data) override;

    //! @copydoc block_device_interface::send()
    bool send(const std::string& data) override;

    //! @copydoc connection_interface::set_lingering_timeout()
    bool set_lingering_timeout(const int32_t& timeout) override;

    //! Connects to the server and returns true, when the connection was
    //! established successfully.
    bool connect(void);

private:
    bool send(const char_t* buffer, const size_t& size);

    bool is_connected_;
    int32_t socket_;
    const sockaddr_in address_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_SOCKET_CONNECTION_HPP
