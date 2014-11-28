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

#ifndef LIBHUTZNOHMD_SOCKET_MOCK_SOCKET_INTERFACE_HPP
#define LIBHUTZNOHMD_SOCKET_MOCK_SOCKET_INTERFACE_HPP

#include <gmock/gmock.h>

#include <rest.hpp>

namespace rest
{

namespace socket
{

class connection_mock : public connection_interface
{
public:
    MOCK_METHOD0(connect, bool());
    MOCK_METHOD0(close, void());
    MOCK_METHOD2(receive, bool(rest::buffer&, const size_t&));
    MOCK_METHOD1(send, bool(const rest::buffer&));
    MOCK_METHOD1(send, bool(const std::string&));
    MOCK_METHOD1(set_lingering_timeout, bool(const int&));
    MOCK_CONST_METHOD0(socket, int());
};

typedef std::shared_ptr<connection_mock> connection_mock_pointer;

class listener_mock : public listener_interface
{
public:
    MOCK_CONST_METHOD0(accept, connection_pointer());
    MOCK_CONST_METHOD0(listening, bool());
    MOCK_METHOD0(stop, void());
    MOCK_METHOD1(set_lingering_timeout, bool(const int&));
    MOCK_CONST_METHOD0(socket, int());
};

} // namespace socket

} // namespace rest

#endif // LIBHUTZNOHMD_SOCKET_MOCK_SOCKET_INTERFACE_HPP
