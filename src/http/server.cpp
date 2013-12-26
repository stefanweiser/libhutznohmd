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

#include <string>

#include <assert.h>
#include <arpa/inet.h>

#include "server.hpp"

namespace rest
{

namespace
{

//! Callback used by libmicrohttpd, when someone wants to connect to us.
int acceptCallback(void * pObject, const sockaddr * addr, socklen_t addrlen)
{
    assert(pObject != nullptr);
    return static_cast<HttpServer *>(pObject)->accept(addr, addrlen);
}

//! Callback used by libmicrohttpd, when someone transmits a request to us.
int accessCallback(void * pObject,
                   MHD_Connection * pConnection,
                   const char * url,
                   const char * method,
                   const char * version,
                   const char * uploadData,
                   size_t * uploadDataSize,
                   void ** ptr)
{
    assert(pObject != nullptr);
    return static_cast<HttpServer *>(pObject)->access(pConnection,
            url,
            method,
            version,
            uploadData,
            uploadDataSize,
            ptr);
}

//! Callback used by libmicrohttpd, when the response of a request was sent.
void completedCallback(void * pObject,
                       MHD_Connection * pConnection,
                       void ** ppData,
                       MHD_RequestTerminationCode reason)
{
    assert(pObject != nullptr);
    return static_cast<HttpServer *>(pObject)->completed(pConnection,
            ppData,
            reason);
}

} // namespace

http::Method convertMethod(const char * const method)
{
    if (std::string(method) == std::string(MHD_HTTP_METHOD_HEAD))
    {
        return http::Method::HEAD;
    }
    else if (std::string(method) == std::string(MHD_HTTP_METHOD_GET))
    {
        return http::Method::GET;
    }
    else if (std::string(method) == std::string(MHD_HTTP_METHOD_PUT))
    {
        return http::Method::PUT;
    }
    else if (std::string(method) == std::string(MHD_HTTP_METHOD_POST))
    {
        return http::Method::POST;
    }
    else if (std::string(method) == std::string(MHD_HTTP_METHOD_DELETE))
    {
        return http::Method::DELETE;
    }
    else if (std::string(method) == std::string(MHD_HTTP_METHOD_TRACE))
    {
        return http::Method::TRACE;
    }
    else if (std::string(method) == std::string(MHD_HTTP_METHOD_OPTIONS))
    {
        return http::Method::OPTIONS;
    }
    else if (std::string(method) == std::string(MHD_HTTP_METHOD_CONNECT))
    {
        return http::Method::CONNECT;
    }
    else
    {
        return http::Method::UNKNOWN;
    }
}

http::Version convertVersion(const char * const version)
{
    if (std::string(version) == std::string(MHD_HTTP_VERSION_1_0))
    {
        return http::Version::HTTP_1_0;
    }
    else if (std::string(version) == std::string(MHD_HTTP_VERSION_1_1))
    {
        return http::Version::HTTP_1_1;
    }
    else
    {
        return http::Version::HTTP_UNKNOWN;
    }
}

unsigned int convertStatusCode(const http::StatusCode & statusCode)
{
    return static_cast<int32_t>(statusCode);
}

HttpServer::HttpServer(const std::string & address,
                       const uint16_t & port,
                       const http::AcceptFn & acceptFn,
                       const http::AccessFn & accessFn)
    : m_pDaemon(nullptr)
    , m_acceptFn(acceptFn)
    , m_accessFn(accessFn)
{
    sockaddr_in addr;

    addr.sin_port   = htons(port);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, address.c_str(), &addr.sin_addr);

    m_pDaemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
                                 port,
                                 &acceptCallback,
                                 this,
                                 &accessCallback,
                                 this,
                                 MHD_OPTION_NOTIFY_COMPLETED,
                                 &completedCallback,
                                 this,
                                 MHD_OPTION_SOCK_ADDR,
                                 &addr,
                                 MHD_OPTION_END);
}

HttpServer::~HttpServer()
{
    MHD_stop_daemon(m_pDaemon);
}

int HttpServer::accept(const sockaddr * addr, socklen_t addrlen)
{
    if (!m_acceptFn ||
            (true == m_acceptFn(addr, addrlen)))
    {
        return MHD_YES;
    }
    return MHD_NO;
}

int HttpServer::access(MHD_Connection * pConnection,
                       const char * url,
                       const char * method,
                       const char * version,
                       const char * uploadData,
                       size_t * uploadDataSize,
                       void ** ptr)
{
    // This method is called several times:
    // 1. Headers are processed.
    // 2. Data gets processed.
    // 3. Creating a response.

    assert(ptr != nullptr);

    if (*ptr == nullptr)
    {
        *ptr = new std::string();
        return MHD_YES;
    }

    std::string * pData = static_cast<std::string *>(*ptr);
    assert(pData != nullptr);

    if (*uploadDataSize > 0)
    {
        pData->append(uploadData, *uploadDataSize);
        *uploadDataSize = 0;
        return MHD_YES;
    }
    else
    {
        return access(pConnection, url, method, version, *pData);
    }
}

int HttpServer::access(MHD_Connection * pConnection,
                       const char * const url,
                       const char * const method,
                       const char * const version,
                       const std::string & uploadData)
{
    std::string data;
    http::StatusCode statusCode = http::StatusCode::InternalServerError;

    if (m_accessFn)
    {
        statusCode = m_accessFn(url,
                                convertMethod(method),
                                convertVersion(version),
                                uploadData,
                                data);
    }
    MHD_Response * pResponse;
    pResponse = MHD_create_response_from_data(data.size(),
                const_cast<char *>(data.c_str()),
                MHD_NO,
                MHD_YES);
    int result = MHD_queue_response(pConnection,
                                    convertStatusCode(statusCode),
                                    pResponse);
    MHD_destroy_response(pResponse);

    return result;
}

void HttpServer::completed(MHD_Connection * /*pConnection*/,
                           void ** ppData,
                           MHD_RequestTerminationCode /*reason*/)
{
    // Cleanup internal string buffer.
    if (ppData != nullptr)
    {
        std::string * p = (std::string *) *ppData;

        if (p != nullptr)
        {
            delete p;
            *ppData = nullptr;
        }
    }
}

HttpServer::HttpServer(const HttpServer & /*rhs*/)
    : m_pDaemon(nullptr)
    , m_acceptFn()
    , m_accessFn()
{}

HttpServer & HttpServer::operator=(const HttpServer & /*rhs*/)
{
    m_pDaemon = nullptr;
    return *this;
}

} // namespace rest
