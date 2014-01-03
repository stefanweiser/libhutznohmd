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

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <socket/connectionsocket.hpp>

#include "utility.hpp"

namespace rest
{

namespace socket
{

::sockaddr_in fillAddress(const std::string & host, const uint16_t & port)
{
    ::sockaddr_in addr;

    if (!::inet_aton(host.c_str(), &addr.sin_addr))
    {
        const ::hostent * hostname = ::gethostbyname(host.c_str());

        if (!hostname)
        {
            throw std::bad_alloc();
        }
        addr.sin_addr = * (::in_addr *) hostname->h_addr;
    }

    addr.sin_port = ::htons(port);
    addr.sin_family = AF_INET;

    return addr;
}

} // namespace socket

} // namespace rest
