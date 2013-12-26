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

#ifndef __LIBREST_HTTP_SERVER_HPP__
#define __LIBREST_HTTP_SERVER_HPP__

#include <functional>

#include <microhttpd.h>

#include <librest.hpp>

namespace rest
{

//! Converts libmicrohttpd methods to librest methods.
http::Method convertMethod(const char * const method);

//! Converts libmicrohttpd versions to librest versions.
http::Version convertVersion(const char * const version);

//! Converts librest status codes into libmicrohttpd status codes.
unsigned int convertStatusCode(const http::StatusCode & statusCode);

//! Opens a port to listen on. Implements HTTP to answer requests. Communicates
//! via callbacks with its user.
class HttpServer : public http::IServer
{
public:
    //! Creates and binds to a port.
    explicit HttpServer(
        const std::string & address,     //! Address to bind to.
        const uint16_t & port,           //! Port to bind to.
        const http::AcceptFn & acceptFn, //! Called if anyone wants to connect.
        const http::AccessFn & accessFn  //! Called in case of a requests.
    );

    //! Destroys the http server.
    virtual ~HttpServer();

    //! Intermediate method to check if the server accepts a connection.
    int accept(const sockaddr * addr,
               socklen_t addrlen);

    //! Internal method to build up the request out of what libmicrohttpd is
    //! giving you.
    int access(MHD_Connection * pConnection,
               const char * url,
               const char * method,
               const char * version,
               const char * uploadData,
               size_t * uploadDataSize,
               void ** ptr);

    //! Intermediate method to build the response for a request.
    int access(MHD_Connection * pConnection,
               const char * const url,
               const char * const method,
               const char * const version,
               const std::string & uploadData);

    //! Called to clean up the request.
    void completed(MHD_Connection * pConnection,
                   void ** ppData,
                   MHD_RequestTerminationCode reason);

private:
    //! Copy constructor.
    explicit HttpServer(const HttpServer & rhs);

    //! Assignment copy.
    HttpServer & operator=(const HttpServer & rhs);

    MHD_Daemon * m_pDaemon;    //!< Internal daemon handle.
    http::AcceptFn m_acceptFn; //!< Callback for accepting connections.
    http::AccessFn m_accessFn; //!< Callback to answer requests.
};

} // namespace rest

#endif // __LIBREST_HTTP_SERVER_HPP__
