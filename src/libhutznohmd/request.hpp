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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_REQUEST_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_REQUEST_HPP

#include <libhutznohmd/sockets.hpp>

namespace hutzn
{

/*!

@page requests Requests

Parsing a request is done by an internal HTTP parser. It offers the server an
optimized view on the request. On the other hand the server has to generate a
response. The following activity diagram is a simplification of the control
flow during request handling:

@startuml{request_activity.svg}

start

:read request;
if (request ok) then (yes)
    if (handler exists) then (yes)
        :call handler;
        :write response;
    else (no)
        if (error handler exists) then (yes)
            :call error handler;
            :write response;
        else (no)
            :write internal server error response;
        endif
    endif
else (no)
    :write bad request error response;
endif

stop

@enduml

Each request consists of verb, path, version, some headers and a body. All of
this is represented in the request interface. All the headers defined by the
current HTTP standard are listed in the following subpages.

Every header field documentation is splitted into three parts:
- @a Description: What and how does it influence the system? What can you
expect? Informations on the content.
- @a Example: A common example on how to use it.
- @a Default: Which value the server supposes, if the header field is missing.
- @a Implemented: Since when is it implemented by the library.

-# @subpage page_basic_http_support
-# @subpage page_encoding
-# @subpage page_auth
-# @subpage page_caching
-# @subpage page_cookies
-# @subpage page_byte_serving
-# @subpage page_conditional

*/

//! This namespace contains all request related code and data. This includes a
//! request (readable) and response (writeable) interface for the server side.
namespace request
{

//! The MIME-Type consists of a group of subtypes.
enum class mime_type : uint8_t {
    //! The character *. Used to catch all types or declaring that the subtype
    //! is not of interest.
    WILDCARD = 0,

    //! "application"
    APPLICATION = 1,

    //! "audio"
    AUDIO = 2,

    //! "image"
    IMAGE = 3,

    //! "text"
    TEXT = 4,

    //! "video"
    VIDEO = 5
};

//! The subtype of a MIME-Type defines the concrete format of the
//! representation.
enum class mime_subtype : uint16_t {
    //! The character *. Used to catch all subtypes or declaring that the
    //! subtype is not of interest.
    WILDCARD = 0,

    //! "plain" means, that the content is not in any specific format.
    PLAIN = 1
};

//! Every HTTP request has a specific method. There are two properties to a
//! subset of methods.
//!
//! -# Idempotence: You can apply the identical request n times to the same URI
//!    without changing the result.
//! -# Side-Effect-Safety: The request does not alter the server state in any
//!    belong. Thus it is inherently idempotent.
//!
//! The user has to ensure, that this properties are fulfilled. It is not easy
//! to solve this by a framework or library.
enum class method : uint8_t {
    //! The GET method is used to retrieve informations from the entity assigned
    //! to the URI. The request must not have side effects.
    GET = 0,

    //! The PUT method requests to store data to the entity represented by the
    //! URI. This request has to be idempotent.
    PUT = 1,

    //! The DELETE method requests that the origin server deletes the resource
    //! identified by the URI. This method has to be idempotent.
    DELETE = 2,

    //! The POST method is used to request that the origin server accepts the
    //! entity enclosed in the request.
    POST = 3
};

//! A request is answered with a response. These responses carry a status code
//! to tell the client the request result. The user is able to define custom
//! status codes to extend this enumeration:
//!
//! @code
//! constexpr hutzn::request::status_code xy =
//!     static_cast<hutzn::request::status_code>(111);
//! @endcode
enum class status_code : uint16_t {
    //! Since HTTP/1.1
    //! Used when dividing a request into several parts. After responding this
    //! code the client shall send more data of the request.
    CONTINUE = 100,

    //! Since HTTP/1.1
    //! Acknowledging the client, that a requested protocol switch is accepted
    //! by the server.
    SWITCHING_PROTOCOLS = 101,

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

    //! The request was processed successfully, but the response does not
    //! contain any informations.
    NO_CONTENT = 204,

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

    //! The requirements of the Expect header cannot be fulfilled.
    EXPECTATION_FAILED = 417,

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

class request_interface;
class response_interface;

} // namespace request

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_REQUEST_HPP
