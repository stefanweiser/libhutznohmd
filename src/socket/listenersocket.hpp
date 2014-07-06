/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBREST_SOCKET_LISTENERSOCKET_HPP__
#define __LIBREST_SOCKET_LISTENERSOCKET_HPP__

#include <cstdint>
#include <memory>
#include <string>

#include <rest.hpp>

#include <socket/socketinterface.hpp>

#include <socket/utility.hpp>

namespace rest
{

namespace socket
{

class listener_socket : public listener_socket_interface
{
public:
    static std::shared_ptr<listener_socket> create(const std::string & host,
            const uint16_t & port);

    explicit listener_socket(const int & s);
    virtual ~listener_socket();
    virtual connection_pointer accept() const;
    virtual bool listening() const;
    virtual void stop();

    int socket() const;

private:
    bool is_listening_;
    int socket_;
};

} // namespace socket

} // namespace rest

#endif // __LIBREST_SOCKET_LISTENERSOCKET_HPP__
