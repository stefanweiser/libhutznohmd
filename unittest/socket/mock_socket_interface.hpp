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

#ifndef LIBREST_SOCKET_MOCK_SOCKET_INTERFACE_HPP
#define LIBREST_SOCKET_MOCK_SOCKET_INTERFACE_HPP

#include <gmock/gmock.h>

#include <socket/socket_interface.hpp>

namespace rest
{

namespace socket
{

class connection_socket_mock : public connection_socket_interface
{
public:
    MOCK_METHOD0(connect, bool());
    MOCK_METHOD0(close, void());
    MOCK_METHOD2(receive, bool(rest::buffer&, const size_t&));
    MOCK_METHOD1(send, bool(const rest::buffer&));
    MOCK_METHOD1(send, bool(const std::string&));
};

typedef std::shared_ptr<connection_socket_mock> connection_mock_pointer;

class listener_socket_mock : public listener_socket_interface
{
public:
    MOCK_CONST_METHOD0(accept, connection_pointer());
    MOCK_METHOD0(stop, void());
};

} // namespace socket

} // namespace rest

#endif // LIBREST_SOCKET_MOCK_SOCKET_INTERFACE_HPP
