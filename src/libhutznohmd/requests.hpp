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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_REQUESTS_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_REQUESTS_HPP

#include <libhutznohmd/sockets.hpp>

namespace hutzn
{

/*!
@page requests Requests

Once you got a connection, your client would propably perform a request. To
parse it and to respond to it correctly a HTTP parser ist needed. HTTP on the
other hand defines access to different documents on the same server. Therefore
a request multiplexer is necessary.

@startuml{requests_classes.png}
namespace hutzn {
  namespace request {
    interface media_type_interface

    class representation_identification {
      +url: string
      +verb: method
      +type: mime_type
      +subtype: mime_subtype
    }

    interface request_parser_interface {
      +handle(connection: connection_interface): boolean
    }

    interface handler_interface

    interface multiplexer_interface {
      +connect(id: representation_identification, fn): handler_interface
      +set_error_handler(code: status_code, fn): handler_interface
    }

    class request_parser
    class handler
    class multiplexer

    handler_interface <|-- handler: <<implements>>
    request_parser_interface <|-- request_parser: <<implements>>
    multiplexer_interface <|-- multiplexer: <<implements>>
  }
}
@enduml

In other words, requests are heading from the connection through the request
parser to the multiplexer, which looks for a representation functor. This
functor is getting called in order to get a response. If no functor could be
found, the request will respond an error document.

A resource starts to exist, when you register the first representation callback
and ceases when the last representation handler gets destroyed. Registering a
resource representation requires to define a callback function that must have
the signature:

@code{.cpp}
status_code foo(const request_interface&, const response_interface&)
@endcode

Because of @c std::bind the user has the choice to use am member function as the
callback function.

@code{.cpp}
std::bind(&Foo::bar, foo_pointer, std::placeholders::_1, std::placeholders::_2);
@endcode

This conceals an important detail from the user. The user exposes the this
pointer of an object. This complicates lifetime. See @ref sec_lifetime_callbacks
"lifetime of callbacks" for further information.
*/

//! This namespace contains all request related code and data. This includes a
//! HTTP parser and a request multiplexer.
namespace request
{

//! The MIME-Type consists of a group and a subtype.
enum class mime_type : uint8_t {
    //! The character *. Used to catch all existing types or declaring
    //! that the subtype is not of interest.
    WILDCARD = 0,

    //! Application specific formats.
    APPLICATION = 1,

    //! An audio format.
    AUDIO = 2,

    //! An image format.
    IMAGE = 3,

    //! A textual format.
    TEXT = 4,

    //! A video format.
    VIDEO = 5
};

//! The subtype of a MIME-Type defines the concrete format of the
//! representation.
enum class mime_subtype : uint16_t {
    //! The character *. Used to catch all existing subtypes or declaring
    //! that the subtype is not of interest.
    WILDCARD = 0,

    //! Plain text means, that the content is not in any specific format.
    PLAIN = 1
};

//! Every HTTP request has a specific method. There are two properties to a
//! subset of methods.
//! 1. Idempotence: You can apply the identical request n times to the same URI
//!    without changing the result.
//! 2. Side-Effect-Safety: The request does not alter the server state. Thus
//!    it is inherently idempotent.
enum class method : uint8_t {
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

//! An request is answered with a response. These responses carry a status code
//! to tell the client the request result.
enum class status_code : uint16_t {
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

struct representation_identification
{
    std::string url;
    method verb;
    mime_type type;
    mime_subtype subtype;
};

class request_parser_interface
{
public:
    virtual ~request_parser_interface();

    virtual bool handle(const socket::connection_interface& connection) = 0;
};

class handler_interface
{
public:
    virtual ~handler_interface();
};

using handler_pointer = std::shared_ptr<handler_interface>;

class request_interface;
class response_interface;

using representation_callback =
    std::function<status_code(const request_interface&, response_interface&)>;
using error_callback =
    std::function<void(const request_interface&, const response_interface&)>;

class multiplexer_interface
{
public:
    virtual ~multiplexer_interface();

    virtual handler_pointer connect(const representation_identification& id,
                                    const representation_callback& fn) = 0;
    virtual handler_pointer set_error_handler(const status_code& code,
                                              const error_callback& fn) = 0;
};

} // namespace request

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_REQUESTS_HPP
