/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#ifndef __LIBREST_SOCKET_LISTENERSOCKET_HPP__
#define __LIBREST_SOCKET_LISTENERSOCKET_HPP__

#include <cstdint>
#include <memory>
#include <string>

#include <librest.hpp>

#include <socket/socketinterface.hpp>

#include <socket/utility.hpp>

namespace rest
{

namespace socket
{

class ListenerSocket : public ListenerSocketInterface
{
public:
    static std::shared_ptr<ListenerSocket> create(
        const std::string & host,
        const uint16_t & port);

    explicit ListenerSocket(const int & socket);
    virtual ~ListenerSocket();
    virtual ConnectionPtr accept();
    virtual bool listening() const;
    virtual void stop();

private:
    bool m_isListening;
    int m_socket;
};

} // namespace socket

} // namespace rest

#endif // __LIBREST_SOCKET_LISTENERSOCKET_HPP__
