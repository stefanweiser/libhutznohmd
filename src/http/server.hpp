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

#ifndef SERVER_HPP
#define SERVER_HPP

#include <functional>

#include <arpa/inet.h>
#include <microhttpd.h>

#include <librest.hpp>

namespace rest
{

typedef std::function<bool(const sockaddr *, const socklen_t)> AcceptFn;
typedef std::function<StatusCode(const char * const url,
                                 const char * const method,
                                 const char * const version,
                                 const std::string& uploadData,
                                 std::string& downloadData)> AccessFn;

class HttpServer
{
public:
    explicit HttpServer(const std::string & address,
                        const uint16_t & port,
                        const AcceptFn& acceptFn,
                        const AccessFn& accessFn);
    ~HttpServer();

    int accept(const sockaddr * addr, socklen_t addrlen);

    int access(MHD_Connection * pConnection,
               const char * url,
               const char * method,
               const char * version,
               const char * uploadData,
               size_t * uploadDataSize,
               void ** ptr);

    int access(MHD_Connection * pConnection,
               const char * const url,
               const char * const method,
               const char * const version,
               const std::string& uploadData);

    void completed(MHD_Connection * pConnection,
                   void ** ppData,
                   MHD_RequestTerminationCode reason);

    explicit HttpServer(const HttpServer & rhs);
    HttpServer & operator=(const HttpServer & rhs);

private:
    MHD_Daemon * m_pDaemon;
    AcceptFn m_acceptFn;
    AccessFn m_accessFn;
};

} // namespace rest

#endif // SERVER_HPP
