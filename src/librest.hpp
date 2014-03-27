/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

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
typedef std::vector<char> buffer;

namespace http
{

//! Every request of HTTP has a specific method.
//! There are two properties to a subset of methods.
//! 1. Idempotence: You can apply the identical request n times to the same URI
//!    without changing the result.
//! 2. Side-Effect-Safety: The request does not alter the server state. Thus
//!    it is inherently idempotent.
enum class method
{
    //! The request is of unknown method. It is recommended to assert this case.
    UNKNOWN = 0,

    //! The HEAD method is identical to GET except that the server must not
    //! return a message-body in the response. The method must not have side
    //! effects.
    HEAD = 1,

    //! The GET method is used to retrieve informations from the entity assigned
    //! to the URI. The method must not have side effects.
    GET = 2,

    //! The PUT method requests to store data to the entity represented by the
    //! URI. This method has to be idempotent.
    PUT = 3,

    //! The DELETE method requests that the origin server deletes the resource
    //! identified by the URI. This method has to be idempotent.
    DELETE = 4,

    //! The POST method is used to request that the origin server accept the
    //! entity enclosed in the request.
    POST = 5,

    //! The TRACE method has to return the message. The client is able to see
    //! whether there are modifications made by intermediate servers.
    TRACE = 6,

    //! The OPTIONS method returns all available methods on the URI.
    OPTIONS = 7,

    //! Converts the request connection to a transparent TCP/IP tunnel, usually
    //! to facilitate SSL-encrypted communication through an unencrypted HTTP
    //! proxy.
    CONNECT = 8
};

//! Defines the HTTP version used by the request.
enum class version
{
    //! The request is of unknown version. It is recommended to assert this
    //! case.
    HTTP_UNKNOWN = 0,

    //! The older version of the HTTP protocol.
    HTTP_1_0 = 1,

    //! Main difference to version 1.0 is that servers on one IP address can
    //! host more than one website. The second difference is that persistent
    //! connections are the default case now and non-persistence must be
    //! explicitly wished.
    HTTP_1_1 = 2
};

//! An request is answered with a response. These responses carry a status code
//! to tell the client the request result.
enum class status_code : int32_t
{
    //! Since HTTP/1.1
    //! Used when dividing a request into several parts. After responding this
    //! the client shall send more data of the request.
    CONTINUE = 100,

    //! Since HTTP/1.1
    //! Acknowledging the client, that a requested protocol switch is accepted
    //! by the server.
    SWITCHING_PROTOCOLS = 101,

    //! Used for WebDAV.
    //! The server responses this, when the server has received the request and
    //! is currently processing it.
    PROCESSING = 102,

    //! Standard response for successful requests.
    OK = 200,

    //! Request resulted in a new resource.
    CREATED = 201,

    //! The server accepted the request. The processing may be not completed,
    //! yet.
    ACCEPTED = 202,

    //! Since HTTP/1.1
    //! The request was processed successfully, but the response contains
    //! informations from another source.
    NON_AUTHORATIVE_INFORMATION = 203,

    //! The request was processes successfully, but the response does not
    //! contain any informations.
    NO_CONTENT = 204,

    //! The server successfully processed the request, but is not returning any
    //! content. Unlike a 204 response, this response requires that the
    //! requester reset the document view.
    RESET_CONTENT = 205,

    //! The server delivers only parts of the resource, because the client
    //! wished so by using a range header (e.g. continuing canceled downloads).
    PARTIAL_CONTENT = 206,

    //! Used for WebDAV.
    //! The body consists of multiple response codes, depending on the number of
    //! sub-requests made.
    MULTI_STATUS = 207,

    //! Indicates multiple options for the client to follow.
    MULTIPLE_CHOICES = 300,

    //! The resource has moved to the given URI.
    MOVED_PERMANENTLY = 301,

    //! Indicates, that the server has found the resource on another URI and
    //! that the client shall redirect to it. Though it is broadly used, it is
    //! not the standard way of redirection.
    FOUND = 302,

    //! Since HTTP/1.1
    //! The standard way of redirecting to another URI.
    SEE_OTHER = 303,

    //! Indicates that the resource has not been modified since the version
    //! specified by the request headers If-Modified-Since or If-Match.
    NOT_MODIFIED = 304,

    //! Since HTTP/1.1
    //! The requested resource is only available via proxy.
    USE_PROXY = 305,

    //! Obsolete.
    SWITCH_PROXY = 306,

    //! Since HTTP/1.1
    //! The resource has moved, but unlike 301 not permanently. Thus future
    //! requests shall use this URI again.
    TEMPORARY_REDIRECT = 307,

    //! The request cannot be parsed due to bad syntax.
    BAD_REQUEST = 400,

    //! Responding the requested information is prohibited, because the client
    //! has not authentificated itself. There must be a WWW-Authenticate headers
    //! with a challenge in the response.
    UNAUTHORIZED = 401,

    //! Reserved for future use.
    PAYMENT_REQUIRED = 402,

    //! Responding the requested information is prohibited, but authentification
    //! will make no difference.
    FORBIDDEN = 403,

    //! The requested resource was not found.
    NOT_FOUND = 404,

    //! The requested resource does not support this method.
    METHOD_NOT_ALLOWED = 405,

    //! The requested resource cannot generate content, due to non-acceptable
    //! Accept headers.
    METHOD_NOT_ACCEPTABLE = 406,

    //! The client must first authenticate itself with a proxy.
    PROXY_AUTHENTIFICATION_REQUIRED = 407,

    //! The server timed out. The client was not able to finish the request
    //! within a server specific timeout.
    REQUEST_TIMEOUT = 408,

    //! The request cannot be processed, due to a conflict within the request.
    //! (e.g. an edit conflict)
    CONFLICT = 409,

    //! Indicates, that the resource was gone and will never be available again.
    GONE = 410,

    //! The length was not specified in the request.
    LENGTH_REQUIRED = 411,

    //! One of the preconditions is failed by the server.
    PRECONDITION_FAILED = 412,

    //! The request cannot be processed, due to its size.
    REQUEST_ENTITY_TOO_LARGE = 413,

    //! The request cannot be processed, due to its URI length.
    REQUEST_URI_TOO_LONG = 414,

    //! The requested media type is not available for the requested resource.
    UNSUPPORTED_MEDIA_TYPE = 415,

    //! The requested range of the requested resource cannot be supplied. (e.g.
    //! beyond the end of file)
    REQUESTED_RANGE_NOT_SATISFIABLE = 416,

    //! The requirements of the Expect header cannot be fulfilled.
    EXPECTATION_FAILED = 417,

    //! Used for WebDAV.
    //! The request cannot be processed, due to semantic errors.
    UNPROCESSABLE_ENTITY = 422,

    //! Used for WebDAV.
    //! The requested resource is locked.
    LOCKED = 423,

    //! Used for WebDAV.
    //! The request failed, due to a failure of a previous request.
    FAILED_DEPENDENCY = 424,

    //! The client should switch to a different protocol.
    UPGRADE_REQUIRED = 426,

    //! A standard response code indicating a server error, where no other
    //! message is suitable.
    INTERNAL_SERVER_ERROR = 500,

    //! The server lacks to fulfill the request. This implies future
    //! availability.
    NOT_IMPLEMENTED = 501,

    //! A gateway server got an invalid response from the upstream server.
    BAD_GATEWAY = 502,

    //! The server is currently unavailable. (e.g. maintainance)
    SERVICE_UNAVAILABLE = 503,

    //! A gateway server did not get a response within time.
    GATEWAY_TIMEOUT = 504,

    //! The server does not support the http procotol version used in the
    //! request.
    HTTP_VERSION_NOT_SUPPORTED = 505
};

//! Represents all data, that depend on the request.
class request_interface
{
public:
    virtual ~request_interface() {}

    //! Requested HTTP method.
    virtual rest::http::method method() const = 0;

    //! Requested URL.
    virtual std::string url() const = 0;

    //! Requested HTTP version.
    virtual rest::http::version version() const = 0;

    //! Returns the header "key" of the request.
    virtual const std::string & header(const std::string & key) const = 0;

    //! Returns the data buffer.
    virtual rest::buffer data() const = 0;
};

//! Represents all data, that get transmitted in the response.
class response_interface
{
public:
    virtual ~response_interface() {}

    //! Sets the status code.
    virtual void set_status_code(const rest::http::status_code & statusCode) = 0;

    //! Sets the HTTP version.
    virtual void set_version(const rest::http::version & version) = 0;

    //! Sets a header by key to a value.
    virtual void set_header(const std::string & key, const std::string & value) = 0;

    //! Sets the content data.
    virtual void set_data(const rest::buffer & data) = 0;
};

//! Callback type, used for every http transaction.
typedef std::function < void(const request_interface & request,
                             response_interface & response) > transaction_function;

//! Represents a HTTP server.
class server_interface
{
public:
    virtual ~server_interface() {}

    //! Runs the server. Won't return till the server shall stop.
    virtual void run() = 0;

    //! Stops the server from accepting further connections.
    virtual void stop() = 0;
};

//! Shortcut typedef for a HTTP server.
typedef std::shared_ptr<server_interface> server_pointer;

//! Creates a server, that listens at a host address on specific port.
server_pointer create_server(const std::string & host,
                             const uint16_t & port,
                             const transaction_function & access_functor);

} // namespace http

} // namespace rest

#endif // __LIBREST_LIBREST_HPP__
