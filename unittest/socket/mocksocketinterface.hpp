/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBREST_SOCKET_MOCKSOCKETINTERFACE_HPP__
#define __LIBREST_SOCKET_MOCKSOCKETINTERFACE_HPP__

#include <gmock/gmock.h>

#include <socket/socketinterface.hpp>

namespace rest
{

namespace socket
{

class MockConnectionSocket : public ConnectionSocketInterface
{
public:
    MOCK_METHOD2(receive, bool(rest::Buffer &, const size_t &));
    MOCK_METHOD1(send, bool(const rest::Buffer &));
    MOCK_METHOD1(send, bool(const std::string &));
};

typedef std::shared_ptr<MockConnectionSocket> MockConnectionPtr;

class MockListenerSocket : public ListenerSocketInterface
{
public:
    MOCK_CONST_METHOD0(accept, ConnectionPtr());
    MOCK_METHOD0(stop, void());
};

} // namespace socket

} // namespace rest

#endif // __LIBREST_SOCKET_MOCKSOCKETINTERFACE_HPP__
