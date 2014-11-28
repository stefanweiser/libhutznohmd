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

#ifndef LIBHUTZNOHMD_SOCKET_LISTENER_SOCKET_HPP
#define LIBHUTZNOHMD_SOCKET_LISTENER_SOCKET_HPP

#include <cstdint>
#include <memory>
#include <string>

#include <rest.hpp>

namespace rest
{

namespace socket
{

class listener : public listener_interface
{
public:
    static std::shared_ptr<listener> create(const std::string& host,
                                            const uint16_t& port);

    explicit listener(const int& s);
    virtual ~listener();
    virtual connection_pointer accept() const;
    virtual bool listening() const;
    virtual void stop();
    virtual bool set_lingering_timeout(const int& timeout);
    virtual int socket() const;

private:
    bool is_listening_;
    int socket_;
};

} // namespace socket

} // namespace rest

#endif // LIBHUTZNOHMD_SOCKET_LISTENER_SOCKET_HPP
