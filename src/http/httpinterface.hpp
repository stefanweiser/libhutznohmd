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

#ifndef __LIBREST_HTTP_HTTPINTERFACE_HPP__
#define __LIBREST_HTTP_HTTPINTERFACE_HPP__

#include <string>

#include <socket/socketinterface.hpp>

#include <librest.hpp>

namespace rest
{

namespace http
{

class RequestInterface
{
public:
    virtual ~RequestInterface() {}
    virtual Method method() const = 0;
    virtual std::string url() const = 0;
    virtual Version version() const = 0;
    virtual std::string header(const std::string & key) const = 0;
    virtual rest::socket::Buffer data() const = 0;
};

class ResponseInterface
{
public:
    virtual ~ResponseInterface() {}
    virtual void setStatusCode(const StatusCode & statusCode) = 0;
    virtual void setVersion(const Version & version) = 0;
    virtual void setHeader(const std::string & key, const std::string & value) = 0;
    virtual void setData(const rest::socket::Buffer & data) = 0;
};

//! Callback type, used for every http transaction.
typedef std::function < void(const RequestInterface & request,
                             ResponseInterface & response) > TransactionFn;

class ServerInterface
{
public:
    virtual ~ServerInterface() {}
    virtual void run() __attribute__((noreturn)) = 0;
};

std::shared_ptr<ServerInterface> createServer(
    const std::string & host,
    const uint16_t & port,
    const TransactionFn & accessFn);

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_HTTPINTERFACE_HPP__
