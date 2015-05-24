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

#ifndef LIBHUTZNOHMD_SOCKET_LISTENER_HPP
#define LIBHUTZNOHMD_SOCKET_LISTENER_HPP

#include <cstdint>
#include <memory>
#include <string>

#include <libhutznohmd/sockets.hpp>

namespace hutzn
{

class listener : public listener_interface
{
public:
    static std::shared_ptr<listener> create(const std::string& host,
                                            const uint16_t& port);

    explicit listener(const int& socket);
    ~listener(void) override;
    connection_pointer accept(void) const override;
    bool listening(void) const override;
    void stop(void) override;
    bool set_lingering_timeout(const int& timeout) override;

private:
    bool is_listening_;
    int socket_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_SOCKET_LISTENER_HPP
