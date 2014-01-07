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

#ifndef __LIBREST_LIBREST_HPP__
#define __LIBREST_LIBREST_HPP__

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace rest
{

//! Universal buffer type of the library.
typedef std::vector<char> Buffer;

namespace http
{

//! Every request of HTTP has a specific method.
//! There are two properties to a subset of methods.
//! 1. Idempotence: You can apply the identical request n times to the same URI
//!    without changing the result.
//! 2. Side-Effect-Safety: The request does not alter the server state. Thus
//!    it is inherently idempotent.
enum class Method
{
    //! The request is of unknown method. It is recommended to assert this case.
    UNKNOWN,

    //! The HEAD method is identical to GET except that the server must not
    //! return a message-body in the response. The method must not have side
    //! effects.
    HEAD,

    //! The GET method is used to retrieve informations from the entity assigned
    //! to the URI. The method must not have side effects.
    GET,

    //! The PUT method requests to store data to the entity represented by the
    //! URI. This method has to be idempotent.
    PUT,

    //! The DELETE method requests that the origin server deletes the resource
    //! identified by the URI. This method has to be idempotent.
    DELETE,

    //! The POST method is used to request that the origin server accept the
    //! entity enclosed in the request.
    POST,

    //! The TRACE method has to return the message. The client is able to see
    //! whether there are modifications made by intermediate servers.
    TRACE,

    //! The OPTIONS method returns all available methods on the URI.
    OPTIONS,

    //! Converts the request connection to a transparent TCP/IP tunnel, usually
    //! to facilitate SSL-encrypted communication through an unencrypted HTTP
    //! proxy.
    CONNECT
};

//! Defines the HTTP version used by the request.
enum class Version
{
    //! The request is of unknown version. It is recommended to assert this
    //! case.
    HTTP_UNKNOWN,

    //! The older version of the HTTP protocol.
    HTTP_1_0,

    //! Main difference to version 1.0 is that servers on one IP address can
    //! host more than one website. The second difference is that persistent
    //! connections are the default case now and non-persistence must be
    //! explicitly wished.
    HTTP_1_1
};

//! An request is answered with a response. These responses carry a status code
//! to tell the client the request result.
enum class StatusCode : int32_t
{
    //! Since HTTP/1.1
    //! Used when dividing a request into several parts. After responding this
    //! the client shall send more data of the request.
    Continue = 100,

    //! Since HTTP/1.1
    //! Acknowledging the client, that a requested protocol switch is accepted
    //! by the server.
    SwitchingProtocols = 101,

    //! Used for WebDAV.
    //! The server responses this, when the server has received the request and
    //! is currently processing it.
    Processing = 102,

    //! Standard response for successful requests.
    Ok = 200,

    //! Request resulted in a new resource.
    Created = 201,

    //! The server accepted the request. The processing may be not completed,
    //! yet.
    Accepted = 202,

    //! Since HTTP/1.1
    //! The request was processed successfully, but the response contains
    //! informations from another source.
    NonAuthorativeInformation = 203,

    //! The request was processes successfully, but the response does not
    //! contain any informations.
    NoContent = 204,

    //! The server successfully processed the request, but is not returning any
    //! content. Unlike a 204 response, this response requires that the
    //! requester reset the document view.
    ResetContent = 205,

    //! The server delivers only parts of the resource, because the client
    //! wished so by using a range header (e.g. continuing canceled downloads).
    PartialContent = 206,

    //! Used for WebDAV.
    //! The body consists of multiple response codes, depending on the number of
    //! sub-requests made.
    MultiStatus = 207,

    //! Indicates multiple options for the client to follow.
    MultipleChoices = 300,

    //! The resource has moved to the given URI.
    MovedPermanently = 301,

    //! Indicates, that the server has found the resource on another URI and
    //! that the client shall redirect to it. Though it is broadly used, it is
    //! not the standard way of redirection.
    Found = 302,

    //! Since HTTP/1.1
    //! The standard way of redirecting to another URI.
    SeeOther = 303,

    //! Indicates that the resource has not been modified since the version
    //! specified by the request headers If-Modified-Since or If-Match.
    NotModified = 304,

    //! Since HTTP/1.1
    //! The requested resource is only available via proxy.
    UseProxy = 305,

    //! Obsolete.
    SwitchProxy = 306,

    //! Since HTTP/1.1
    //! The resource has moved, but unlike 301 not permanently. Thus future
    //! requests shall use this URI again.
    TemporaryRedirect = 307,

    //! The request cannot be parsed due to bad syntax.
    BadRequest = 400,

    //! Responding the requested information is prohibited, because the client
    //! has not authentificated itself. There must be a WWW-Authenticate headers
    //! with a challenge in the response.
    Unauthorized = 401,

    //! Reserved for future use.
    PaymentRequired = 402,

    //! Responding the requested information is prohibited, but authentification
    //! will make no difference.
    Forbidden = 403,

    //! The requested resource was not found.
    NotFound = 404,

    //! The requested resource does not support this method.
    MethodNotAllowed = 405,

    //! The requested resource cannot generate content, due to non-acceptable
    //! Accept headers.
    MethodNotAcceptable = 406,

    //! The client must first authenticate itself with a proxy.
    ProxyAuthentificationRequired = 407,

    //! The server timed out. The client was not able to finish the request
    //! within a server specific timeout.
    RequestTimeout = 408,

    //! The request cannot be processed, due to a conflict within the request.
    //! (e.g. an edit conflict)
    Conflict = 409,

    //! Indicates, that the resource was gone and will never be available again.
    Gone = 410,

    //! The length was not specified in the request.
    LengthRequired = 411,

    //! One of the preconditions is failed by the server.
    PreconditionFailed = 412,

    //! The request cannot be processed, due to its size.
    RequestEntityTooLarge = 413,

    //! The request cannot be processed, due to its URI length.
    RequestUriTooLong = 414,

    //! The requested media type is not available for the requested resource.
    UnsupportedMediaType = 415,

    //! The requested range of the requested resource cannot be supplied. (e.g.
    //! beyond the end of file)
    RequestedRangeNotSatisfiable = 416,

    //! The requirements of the Expect header cannot be fulfilled.
    ExpectationFailed = 417,

    //! Used for WebDAV.
    //! The request cannot be processed, due to semantic errors.
    UnprocessableEntity = 422,

    //! Used for WebDAV.
    //! The requested resource is locked.
    Locked = 423,

    //! Used for WebDAV.
    //! The request failed, due to a failure of a previous request.
    FailedDependency = 424,

    //! The client should switch to a different protocol.
    UpgradeRequired = 426,

    //! A standard response code indicating a server error, where no other
    //! message is suitable.
    InternalServerError = 500,

    //! The server lacks to fulfill the request. This implies future
    //! availability.
    NotImplemented = 501,

    //! A gateway server got an invalid response from the upstream server.
    BadGateway = 502,

    //! The server is currently unavailable. (e.g. maintainance)
    ServiceUnavailable = 503,

    //! A gateway server did not get a response within time.
    GatewayTimeout = 504,

    //! The server does not support the http procotol version used in the
    //! request.
    HttpVersionNotSupported = 505
};

//! Represents all data, that depend on the request.
class RequestInterface
{
public:
    virtual ~RequestInterface() {}

    //! Requested HTTP method.
    virtual Method method() const = 0;

    //! Requested URL.
    virtual std::string url() const = 0;

    //! Requested HTTP version.
    virtual Version version() const = 0;

    //! Returns the header "key" of the request.
    virtual const std::string & header(const std::string & key) const = 0;

    //! Returns the data buffer.
    virtual rest::Buffer data() const = 0;
};

//! Represents all data, that get transmitted in the response.
class ResponseInterface
{
public:
    virtual ~ResponseInterface() {}

    //! Sets the status code.
    virtual void setStatusCode(const StatusCode & statusCode) = 0;

    //! Sets the HTTP version.
    virtual void setVersion(const Version & version) = 0;

    //! Sets a header by key to a value.
    virtual void setHeader(const std::string & key, const std::string & value) = 0;

    //! Sets the content data.
    virtual void setData(const rest::Buffer & data) = 0;
};

//! Callback type, used for every http transaction.
typedef std::function < void(const RequestInterface & request,
                             ResponseInterface & response) > TransactionFn;

//! Represents a HTTP server.
class ServerInterface
{
public:
    virtual ~ServerInterface() {}

    //! Runs the server. Won't return till the server shall stop.
    virtual void run() = 0;

    //! Stops the server from accepting further connections.
    virtual void stop() = 0;
};

//! Shortcut typedef for a HTTP server.
typedef std::shared_ptr<ServerInterface> ServerPtr;

//! Creates a server, that listens at a host address on specific port.
ServerPtr createServer(
    const std::string & host,
    const uint16_t & port,
    const TransactionFn & accessFn);

} // namespace http

} // namespace rest

#endif // __LIBREST_LIBREST_HPP__
