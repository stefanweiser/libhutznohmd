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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_SOCKETS_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_SOCKETS_HPP

#include <gmock/gmock.h>

#include <hutzn.hpp>

namespace hutzn
{

class connection_interface_mock : public connection_interface
{
public:
    MOCK_METHOD0(connect, bool(void));
    MOCK_METHOD0(close, void(void));
    MOCK_METHOD2(receive, bool(buffer&, const size_t&));
    MOCK_METHOD1(send, bool(const buffer&));
    MOCK_METHOD1(send, bool(const std::string&));
    MOCK_METHOD1(set_lingering_timeout, bool(const int32_t&));
};

using connection_mock_pointer = std::shared_ptr<connection_interface_mock>;

class listener_interface_mock : public listener_interface
{
public:
    MOCK_CONST_METHOD0(accept, connection_pointer(void));
    MOCK_CONST_METHOD0(listening, bool(void));
    MOCK_METHOD0(stop, void(void));
    MOCK_METHOD1(set_lingering_timeout, bool(const int32_t&));
};

using listener_mock_pointer = std::shared_ptr<listener_interface_mock>;

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_SOCKETS_HPP
