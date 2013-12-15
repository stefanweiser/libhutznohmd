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

#include "server.hpp"

namespace rest
{

namespace
{

//! Converts libmicrohttpd methods to librest methods.
Method convertMethod(const char* const method)
{
    if ( std::string(method) == std::string(MHD_HTTP_METHOD_HEAD) )
    {
        return Method::HEAD;
    }
    else if ( std::string(method) == std::string(MHD_HTTP_METHOD_GET) )
    {
        return Method::GET;
    }
    else if ( std::string(method) == std::string(MHD_HTTP_METHOD_PUT) )
    {
        return Method::PUT;
    }
    else if ( std::string(method) == std::string(MHD_HTTP_METHOD_POST) )
    {
        return Method::POST;
    }
    else if ( std::string(method) == std::string(MHD_HTTP_METHOD_DELETE) )
    {
        return Method::DELETE;
    }
    else if ( std::string(method) == std::string(MHD_HTTP_METHOD_TRACE) )
    {
        return Method::TRACE;
    }
    else if ( std::string(method) == std::string(MHD_HTTP_METHOD_OPTIONS) )
    {
        return Method::OPTIONS;
    }
    else if ( std::string(method) == std::string(MHD_HTTP_METHOD_CONNECT) )
    {
        return Method::CONNECT;
    }
    else
    {
        return Method::UNKNOWN;
    }
}

//! Converts libmicrohttpd versions to librest versions.
Version convertVersion(const char* const version)
{
    if ( std::string(version) == std::string(MHD_HTTP_VERSION_1_0) )
    {
        return Version::HTTP_1_0;
    }
    else if ( std::string(version) == std::string(MHD_HTTP_VERSION_1_1) )
    {
        return Version::HTTP_1_1;
    }
    else
    {
        return Version::HTTP_UNKNOWN;
    }
}

//! Converts librest status codes into libmicrohttpd status codes.
unsigned int convertStatusCode(const StatusCode& statusCode)
{
    switch ( statusCode )
    {
    case StatusCode::Continue:
        return MHD_HTTP_CONTINUE;

    case StatusCode::SwitchingProtocols:
        return MHD_HTTP_SWITCHING_PROTOCOLS;

    case StatusCode::Processing:
        return MHD_HTTP_PROCESSING;

    case StatusCode::Ok:
        return MHD_HTTP_OK;

    case StatusCode::Created:
        return MHD_HTTP_CREATED;

    case StatusCode::Accepted:
        return MHD_HTTP_ACCEPTED;

    case StatusCode::NonAuthorativeInformation:
        return MHD_HTTP_NON_AUTHORITATIVE_INFORMATION;

    case StatusCode::NoContent:
        return MHD_HTTP_NO_CONTENT;

    case StatusCode::ResetContent:
        return MHD_HTTP_RESET_CONTENT;

    case StatusCode::PartialContent:
        return MHD_HTTP_PARTIAL_CONTENT;

    case StatusCode::MultiStatus:
        return MHD_HTTP_MULTI_STATUS;

    case StatusCode::MultipleChoices:
        return MHD_HTTP_MULTIPLE_CHOICES;

    case StatusCode::MovedPermanently:
        return MHD_HTTP_MOVED_PERMANENTLY;

    case StatusCode::Found:
        return MHD_HTTP_FOUND;

    case StatusCode::SeeOther:
        return MHD_HTTP_SEE_OTHER;

    case StatusCode::NotModified:
        return MHD_HTTP_NOT_MODIFIED;

    case StatusCode::UseProxy:
        return MHD_HTTP_USE_PROXY;

    case StatusCode::SwitchProxy:
        return MHD_HTTP_SWITCH_PROXY;

    case StatusCode::TemporaryRedirect:
        return MHD_HTTP_TEMPORARY_REDIRECT;

    case StatusCode::BadRequest:
        return MHD_HTTP_BAD_REQUEST;

    case StatusCode::Unauthorized:
        return MHD_HTTP_UNAUTHORIZED;

    case StatusCode::PaymentRequired:
        return MHD_HTTP_PAYMENT_REQUIRED;

    case StatusCode::Forbidden:
        return MHD_HTTP_FORBIDDEN;

    case StatusCode::NotFound:
        return MHD_HTTP_NOT_FOUND;

    case StatusCode::MethodNotAllowed:
        return MHD_HTTP_METHOD_NOT_ALLOWED;

    case StatusCode::MethodNotAcceptable:
        return MHD_HTTP_METHOD_NOT_ACCEPTABLE;

    case StatusCode::ProxyAuthentificationRequired:
        return MHD_HTTP_PROXY_AUTHENTICATION_REQUIRED;

    case StatusCode::RequestTimeout:
        return MHD_HTTP_REQUEST_TIMEOUT;

    case StatusCode::Conflict:
        return MHD_HTTP_CONFLICT;

    case StatusCode::Gone:
        return MHD_HTTP_GONE;

    case StatusCode::LengthRequired:
        return MHD_HTTP_LENGTH_REQUIRED;

    case StatusCode::PreconditionFailed:
        return MHD_HTTP_PRECONDITION_FAILED;

    case StatusCode::RequestEntityTooLarge:
        return MHD_HTTP_REQUEST_ENTITY_TOO_LARGE;

    case StatusCode::RequestUriTooLong:
        return MHD_HTTP_REQUEST_URI_TOO_LONG;

    case StatusCode::UnsupportedMediaType:
        return MHD_HTTP_UNSUPPORTED_MEDIA_TYPE;

    case StatusCode::RequestedRangeNotSatisfiable:
        return MHD_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE;

    case StatusCode::ExpectationFailed:
        return MHD_HTTP_EXPECTATION_FAILED;

    case StatusCode::UnprocessableEntity:
        return MHD_HTTP_UNPROCESSABLE_ENTITY;

    case StatusCode::Locked:
        return MHD_HTTP_LOCKED;

    case StatusCode::FailedDependency:
        return MHD_HTTP_FAILED_DEPENDENCY;

    case StatusCode::UnorderedCollection:
        return MHD_HTTP_UNORDERED_COLLECTION;

    case StatusCode::UpgradeRequired:
        return MHD_HTTP_UPGRADE_REQUIRED;

    case StatusCode::NoResponse:
        return MHD_HTTP_NO_RESPONSE;

    case StatusCode::RetryWith:
        return MHD_HTTP_RETRY_WITH;

    case StatusCode::BlockedByWindowsParentalControls:
        return MHD_HTTP_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS;

    case StatusCode::UnavailableForLegalReasons:
        return MHD_HTTP_UNAVAILABLE_FOR_LEGAL_REASONS;

    case StatusCode::InternalServerError:
        return MHD_HTTP_INTERNAL_SERVER_ERROR;

    case StatusCode::NotImplemented:
        return MHD_HTTP_NOT_IMPLEMENTED;

    case StatusCode::BadGateway:
        return MHD_HTTP_BAD_GATEWAY;

    case StatusCode::ServiceUnavailable:
        return MHD_HTTP_SERVICE_UNAVAILABLE;

    case StatusCode::GatewayTimeout:
        return MHD_HTTP_GATEWAY_TIMEOUT;

    case StatusCode::HttpVersionNotSupported:
        return MHD_HTTP_HTTP_VERSION_NOT_SUPPORTED;

    case StatusCode::HttpVariantAlsoNegotiates:
        return MHD_HTTP_VARIANT_ALSO_NEGOTIATES;

    case StatusCode::InsufficientStorage:
        return MHD_HTTP_INSUFFICIENT_STORAGE;

    case StatusCode::BandwidthLimitExceeded:
        return MHD_HTTP_BANDWIDTH_LIMIT_EXCEEDED;

    case StatusCode::NotExtended:
        return MHD_HTTP_NOT_EXTENDED;

    default:
        return -1;
    }
}

//! Callback used by libmicrohttpd, when someone wants to connect to us.
int acceptCallback(void * pObject, const sockaddr * addr, socklen_t addrlen)
{
    assert(pObject != nullptr);
    return static_cast<HttpServer *>(pObject)->accept(addr, addrlen);
}

//! Callback used by libmicrohttpd, when someone transmits a request to us.
int accessCallback(void           * pObject,
                   MHD_Connection * pConnection,
                   const char     * url,
                   const char     * method,
                   const char     * version,
                   const char     * uploadData,
                   size_t         * uploadDataSize,
                   void          ** ptr)
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
void completedCallback(void                     * pObject,
                       MHD_Connection           * pConnection,
                       void                    ** ppData,
                       MHD_RequestTerminationCode reason)
{
    assert(pObject != nullptr);
    return static_cast<HttpServer *>(pObject)->completed(pConnection,
                                                         ppData,
                                                         reason);
}

} // namespace

HttpServer::HttpServer(const std::string& address,
                       const uint16_t   & port,
                       const AcceptFn   & acceptFn,
                       const AccessFn   & accessFn)
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
    if ( !m_acceptFn ||
         (true == m_acceptFn(addr, addrlen)))
    {
        return MHD_YES;
    }
    return MHD_NO;
}

int HttpServer::access(MHD_Connection * pConnection,
                       const char     * url,
                       const char     * method,
                       const char     * version,
                       const char     * uploadData,
                       size_t         * uploadDataSize,
                       void          ** ptr)
{
    // This method is called several times:
    // 1. Headers are processed.
    // 2. Data gets processed.
    // 3. Creating a response.

    // Ignore head.
    if ( std::string(method) == MHD_HTTP_METHOD_HEAD ) return MHD_YES;

    assert(ptr != nullptr);

    if ( *ptr == nullptr )
    {
        *ptr = new std::string();
        return MHD_YES;
    }

    std::string * pData = static_cast<std::string *>(*ptr);
    assert(pData != nullptr);

    if ( *uploadDataSize > 0 )
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

int HttpServer::access(MHD_Connection   * pConnection,
                       const char * const url,
                       const char * const method,
                       const char * const version,
                       const std::string& uploadData)
{
    std::string data;
    StatusCode  statusCode = StatusCode::InternalServerError;

    if ( m_accessFn )
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
                           void        ** ppData,
                           MHD_RequestTerminationCode /*reason*/)
{
    // Cleanup internal string buffer.
    if ( ppData != nullptr )
    {
        std::string * p = (std::string *)*ppData;

        if ( p != nullptr )
        {
            delete p;
            *ppData = nullptr;
        }
    }
}

HttpServer::HttpServer(const HttpServer& rhs)
    : m_pDaemon(rhs.m_pDaemon)
      , m_acceptFn(rhs.m_acceptFn)
      , m_accessFn(rhs.m_accessFn)
{}

HttpServer& HttpServer::operator=(const HttpServer& rhs)
{
    m_pDaemon  = rhs.m_pDaemon;
    m_acceptFn = rhs.m_acceptFn;
    m_accessFn = rhs.m_accessFn;
    return *this;
}

} // namespace rest
