/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

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
#include <libhutznohmd/types.hpp>

namespace hutzn
{

/*!

@page page_requests Requests

Parsing a request is done by an internal HTTP parser. It offers the server an
optimized view on the request. The server also has to generate a response. The
following activity diagram is a simplification of the control flow during
request handling:

@startuml{request_activity.svg}
start
:read request;
if (request ok?) then (yes)
    if (handler exists?) then (yes)
        :call handler;
        :write response;
    else (no)
        if (error handler exists?) then (yes)
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
this is represented in the request interface.

@startuml{request_classes.svg}
namespace hutzn {
  enum http_verb {
    GET
    PUT
    DELETE
    POST
  }

  enum http_version {
    HTTP_UNKNOWN
    HTTP_1_0
    HTTP_1_1
    HTTP_2_0
  }

  enum http_status_code {
    CONTINUE
    SWITCHING_PROTOCOLS
    OK
    CREATED
    ACCEPTED
    NON_AUTHORATIVE_INFORMATION
    NO_CONTENT
    MULTIPLE_CHOICES
    MOVED_PERMANENTLY
    FOUND
    SEE_OTHER
    NOT_MODIFIED
    USE_PROXY
    TEMPORARY_REDIRECT
    BAD_REQUEST
    UNAUTHORIZED
    FORBIDDEN
    NOT_FOUND
    METHOD_NOT_ALLOWED
    METHOD_NOT_ACCEPTABLE
    PROXY_AUTHENTIFICATION_REQUIRED
    REQUEST_TIMEOUT
    CONFLICT
    GONE
    LENGTH_REQUIRED
    PRECONDITION_FAILED
    REQUEST_ENTITY_TOO_LARGE
    REQUEST_URI_TOO_LONG
    UNSUPPORTED_MEDIA_TYPE
    EXPECTATION_FAILED
    UPGRADE_REQUIRED
    INTERNAL_SERVER_ERROR
    NOT_IMPLEMENTED
    BAD_GATEWAY
    SERVICE_UNAVAILABLE
    GATEWAY_TIMEOUT
    HTTP_VERSION_NOT_SUPPORTED
  }

  enum http_status_code {
    UNKNOWN
    CONTINUE
  }

  interface request_interface {
    +method(): http_verb
    +path(): string
    +version(): http_version
    +keeps_connection(): bool
    +date(): time
    +content(): buffer
    +content_length(): size
    +content_type(): mime
    +accept(in/out handle: pointer, type: mime): bool
    +expect(): http_expectation
    +from(): string
    +referer(): string
    +user_agent(): string
  }

  interface response_interface {
    +set_status_code(status_code: http_status_code)
    +set_version(version: http_version)
    +set_header(key: string, value: string)
    +set_content(content: buffer)
  }

  class request
  class response

  http_version -- request_interface: < uses
  http_verb -- request_interface: < uses
  http_expectation -- request_interface: < uses
  http_version -- response_interface: < uses
  http_status_code -- response_interface: < uses
  request_interface <|-- request: implements
  response_interface <|-- response: implements
}
@enduml

A part of all headers defined by the current HTTP standard are listed. Every
header field documentation is splitted into three parts:
- @a Description: What and how does it influence the system? What could be
expected? Informations on the content.
- @a Example: A common example on how to use it.
- @a Default: Which value the server supposes, if the header field is missing.
- @a Implementation @a Status: Since when is it implemented by the library.

@section sec_basic_request_and_response Request- and Response-Specific

@subsection sub_content_length Content-Length

The header field of the @c Content-Length must be set on any request or
response, that takes content. It must then contain the size of the content in
bytes. Thus it must be an unsigned integer. The size is limited to
\f$2^{31}-1\f$. In case of an overflow the request is getting invalid and
rejected.

The content length could be retrieved by @ref
request_interface::content_length and is set on a response
automatically when a content is set by @ref
response_interface::set_content.

@subsubsection subsub_content_length_example Example:

@code
Content-Length: 123
@endcode

@subsubsection subsub_content_length_default Default:

@code
Content-Length: 0
@endcode

@subsubsection subsub_content_length_implemented Implementation Status:

unimplemented

@subsection sub_content_md5 Content-MD5

This header field is optional and carries a MD5 hash sum. This hash sum is
automatically used to check the content for transmission errors. If the header
field is missing, the content is not verified with MD5. To enable this header
on the response, set the second parameter of @ref
response_interface::set_content to true.

@note For two reasons this is no security feature:
-# An attacker that is able to modify the header field or the content is
everytime also able to modify both.
-# Collisions for MD5 could be found within reasonable time by recent hardware.

@subsubsection subsub_content_md5_example Example:

@code
Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==
@endcode

@subsubsection subsub_content_md5_default Default:

not present

@subsubsection subsub_content_md5_implemented Implementation Status:

unimplemented

@subsection sub_content_type Content-Type

A content type header defines how the application should interpret the content.
It consists of a MIME type. The request processor use this header field to
select the right request handler. The response also takes a content type. These
types are set and used automatically by the request processor.

@subsubsection subsub_content_type_example Example:

@code
Content-Type: application/xml
@endcode

@subsubsection subsub_content_type_default Default:

@code
Content-Type: text/plain
@endcode

@subsubsection subsub_content_type_implemented Implementation Status:

unimplemented

@subsection sub_date Date

Contains the timestamp, when the message was generated. The request processor
will also automatically add a timestamp to the response, pointing to the time
when the response was generated.

@subsubsection subsub_date_example Example:

@code
Date: Wed, 13 May 2014 22:10:48 GMT
@endcode

@subsubsection subsub_date_default Default:

@code
Date: Thu, 01 Jan 1970 00:00:00 GMT
@endcode

@subsubsection subsub_date_implemented Implementation Status:

unimplemented

@section sec_basic_request Request-Specific

@subsection sub_accept Accept

Defines preferable response representations. The content of this header field is
a list of MIME types, which are sorted by a q-value (quality). The higher the
quality value is, the more preferable it is for the server to returns such
a resource represenation. If two MIME types have the same quality values, the
first one is more preferable (although this is not defined by the HTTP/1.1
standard). If no quality value is defined, the highest value of 1.0 is assumed.

Internally it is getting matched against the available request handler for this
resource to pick up the most preferable resource representation that is
available.

@subsubsection subsub_accept_example Example:

@code
Accept: text/plain;q=0.9,text/html;q=0.1
@endcode

@subsubsection subsub_accept_default Default:

@code
Accept: * / *
@endcode

@subsubsection subsub_accept_implemented Implementation Status:

unimplemented

@subsection sub_connection Connection

Determines, what to do with the connection after the request is finished. There
is only one possible value: @c close.

In case of @c close the connection will be closed after the response is sent.
This was the default behaviour on @c HTTP/1.0. Default on @c HTTP/1.1 is now to
keep the connection alive till the client closes the connection or a predefined
time elapsed without activity on the connection.

This time duration can be configured for the request processor in its factory
function @ref make_request_processor. It shall be deemed to be a lower
bound. It could take longer till the connection is effectively getting closed.
See @ref sec_lifetime_connection for more information.

@subsubsection subsub_connection_example Example:

@code
Connection: close
@endcode

@subsubsection subsub_connection_default Default:

till @c HTTP/1.0:
not present, closes the connection immediately

since @c HTTP/1.1:
not present, keeps the connection open till client closes or timeout

@subsubsection subsub_connection_implemented Implementation Status:

unimplemented

@subsection sub_expect Expect

Contains expectations of the client about the response. If the server does not
understand the expectation or cannot fulfill it, it will respond with the error
code 417 (Expectation failed).

Currently only the expectation "100-continue" is defined. It is used to check
validity of the request header before sending the payload data. This propably
saves bandwidth if the server would not accept the request.

@subsubsection subsub_expect_example Example:

@code
Expect: 100-continue
@endcode

@subsubsection subsub_expect_default Default:

not present

@subsubsection subsub_expect_implemented Implementation Status:

unimplemented

@subsection sub_from From

Contains an e-mail address of a human, that controls the requesting user agent.

@subsubsection subsub_from_example Example:

@code
From: user@example.com
@endcode

@subsubsection subsub_from_default Default:

not present

@subsubsection subsub_from_implemented Implementation Status:

unimplemented

@subsection sub_host Host

Refers to the requested host on the server in case of multiple domains hosted on
the server. Therefore it is part of the request URI. If the header is present,
the content may overwrite the content of the request URI.

@subsubsection subsub_host_example Example:

@code
Host: example.com
@endcode

@subsubsection subsub_host_default Default:

not present

@subsubsection subsub_host_implemented Implementation Status:

unimplemented

@subsection sub_referer Referer

Contains the URI, from which the request URI was obtained from.

@subsubsection subsub_referer_example Example:

@code
Referer: http://www.example.com/
@endcode

@subsubsection subsub_referer_default Default:

not present

@subsubsection subsub_referer_implemented Implementation Status:

unimplemented

@subsection sub_user_agent User-Agent

Contains informations about the requesting client product.

@subsubsection subsub_user_agent_example Example:

@code
User-Agent: Googlebot/2.1 (+http://www.google.com/bot.html)
@endcode

@subsubsection subsub_user_agent_default Default:

@code
User-Agent:
@endcode

@subsubsection subsub_user_agent_implemented Implementation Status:

unimplemented

@section sec_basic_response Response-Specific

@subsection sub_allow Allow

Contains all available methods for the requested resource. This field is
automatically added by the request processor in responses with the error code of
405 (Method not allowed).

@subsubsection subsub_allow_example Example:

@code
Allow: GET, HEAD, PUT
@endcode

@subsubsection subsub_allow_default Default:

not present

@subsubsection subsub_allow_implemented Implementation Status:

unimplemented

@subsection sub_content_location Content-Location

Points to the original URI path of the requested resource. Should be set by
calling @ref response_interface::set_content_location, when a
resource is available under different URIs and the requested URI path is not the
primary URI.

@subsubsection subsub_content_location_example Example:

@code
Content-Location: /example
@endcode

@subsubsection subsub_content_location_default Default:

not present

@subsubsection subsub_content_location_implemented Implementation Status:

unimplemented

@subsection sub_location Location

Contains the location the client should be guided to. Is set, when @ref
response_interface::set_location is called and could contain the
URI of a created resource or the URI of a moved one.

@subsubsection subsub_location_example Example:

@code
Location: http://example.com/example
@endcode

@subsubsection subsub_location_default Default:

not present

@subsubsection subsub_location_implemented Implementation Status:

unimplemented

@subsection sub_retry_after Retry-After

Tells the client, that it should repeat the request later to get a proper
result. Is set, when @ref response_interface::set_retry_after
is getting called with a time greater 0 and cleared if called with 0.

@subsubsection subsub_retry_after_example Example:

@code
Retry-After: Fri, 31 Dec 1999 23:59:59 GMT
Retry-After: 120
@endcode

@subsubsection subsub_retry_after_default Default:

not present

@subsubsection subsub_retry_after_implemented Implementation Status:

unimplemented

@subsection sub_server Server

Set to the server/version fingerprint. This is deactivated per default for
security reasons and can be activated by calling @ref
response_interface::set_server on the response.

@subsubsection subsub_server_example Example:

@code
Server: my_server/1.0.0
@endcode

@subsubsection subsub_server_default Default:

not present

@subsubsection subsub_server_implemented Implementation Status:

unimplemented

*/

//! The MIME-Type consists of a group of subtypes.
enum class mime_type : uint8_t {
    //! The type is invalid. The implementation should reject the request or
    //! response.
    INVALID = 0,

    //! No type available.
    NONE,

    //! The character *. Used to catch all types or declaring that the subtype
    //! is not of interest.
    WILDCARD,

    //! Used internally to count the number of initial elements.
    COUNT
};

//! The subtype of a MIME-Type defines the concrete format of the
//! representation.
enum class mime_subtype : uint16_t {
    //! The subtype is invalid. The implementation should reject the request or
    //! response.
    INVALID = 0,

    //! No subtype available.
    NONE,

    //! The character *. Used to catch all subtypes or declaring that the
    //! subtype is not of interest.
    WILDCARD,

    //! Used internally to count the number of initial elements.
    COUNT
};

//! Used to describe a MIME type combination. Often this type is used.
using mime = std::pair<mime_type, mime_subtype>;

//! Every request is marked with a HTTP version number. The sever has to support
//! the behaviour of this version.
enum class http_version : uint8_t {
    //! The request is of unknown version. A request with that version number
    //! could be handled as malformed.
    HTTP_UNKNOWN = 0,

    //! The older version of the HTTP protocol.
    HTTP_1_0 = 1,

    //! The main difference to version 1.0 is that servers on one IP address can
    //! host more than one website. The second difference is that persistent
    //! connection is the default case now and non-persistence must be
    //! explicitly wished.
    HTTP_1_1 = 2,

    //! This version is currently unsupported, but already reserved.
    HTTP_2 = 3
};

//! Every HTTP request has a specific verb. There are two properties to a subset
//! of methods.
//!
//! -# Idempotence: The user can apply the identical request n times to the same
//!    URI without changing the result.
//! -# Side-Effect-Safety: The request does not alter the server state in any
//!    belong. Thus it is inherently idempotent.
//!
//! The user has to ensure, that this properties are fulfilled. It is not easy
//! to solve this by a framework or library.
enum class http_verb : uint8_t {
    //! The verb GET is used to retrieve informations from the entity assigned
    //! to the URI. The request must not have side effects.
    GET = 0,

    //! The verb PUT requests to store data to the entity represented by the
    //! URI. The request has to be idempotent.
    PUT = 1,

    //! The verb DELETE requests that the origin server deletes the resource
    //! identified by the URI. The request has to be idempotent.
    DELETE = 2,

    //! The verb POST is used to request that the origin server accepts the
    //! entity enclosed in the request.
    POST = 3
};

//! A request is answered with a response. These responses carry a status code
//! to tell the client the request result. The user is able to define custom
//! status codes to extend this enumeration. Be aware, that there is no
//! "speaking" status code in the response in that case:
//!
//! @code{.cpp}
//! constexpr http_status_code xy =
//!     static_cast<http_status_code>(111);
//! @endcode
enum class http_status_code : uint16_t {
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

    //! The requested resource does not support the given verb.
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

//! States the HTTP expectation extension of the expect header field. If
//! the expect content is not known, use UNKNOWN.
enum class http_expectation : uint8_t {
    //! The client expects an extension the server does not support.
    UNKNOWN = 0,

    //! The client expects status code 100. This is used to check, whether a
    //! request would be processed or rejected before sending huge packets.
    CONTINUE = 1
};

//! These URI schemes are supported by the parser and needed for HTTP.
enum class uri_scheme : uint8_t {
    //! Scheme is not known or invalid.
    UNKNOWN = 0,

    //! URI describes a URL as used in HTTP for plain data transport.
    HTTP = 1,

    //! URI describes a URL as used in HTTP within an encrypted channel.
    HTTPS = 2,

    //! URI describes an e-mail address.
    MAILTO = 3
};

//! Is used by the request processor to find the right request handler and
class request_interface
{
public:
    virtual ~request_interface(void);

    //! Returns the HTTP verb used by the request (GET, PUT, DELETE or POST are
    //! allowed).
    virtual http_verb method(void) const = 0;

    //! The used URL without scheme, authorization, host, port, queries or
    //! fragment.
    virtual const char_t* path(void) const = 0;

    //! The host name used by the request client. It could be part of the
    //! request line or the header field Host. If a host is present at both
    //! places, the header field Host overwrites that one in the request line.
    //! This is currently just an information.
    virtual const char_t* host(void) const = 0;

    //! Returns a value of a key, that is in the query part of the URL.
    virtual const char_t* query(const char_t* const key) const = 0;

    //! Returns the fragment of the URL.
    virtual const char_t* fragment(void) const = 0;

    //! Returns the used HTTP version. This influences server behaviour
    //! (e.g. connection duration).
    virtual http_version version(void) const = 0;

    //! Returns the content of any custom header field. Only those, which are
    //! available by explicit member functions are available. If the header
    //! field is not present, it returns an empty string. Will never return
    //! a null-pointer.
    virtual const char_t* header_value(const char_t* const name) const = 0;

    //! Returns true, if the connection will be kept after the request is
    //! processed.
    virtual bool keeps_connection(void) const = 0;

    //! Returns the timestamp of the request (date header) as epoch time or 0
    //! if the header field does not exist.
    virtual time_t date(void) const = 0;

    //! Returns a pointer to the begin of the content in the buffer. This data
    //! buffer is @b not null-terminated and has the length of the
    //! content-length header field. If there is no content it returns nullptr.
    virtual void* content(void) const = 0;

    //! Returns the length of the buffer returned by
    //! @ref request_interface::content(). If there is no content it returns 0.
    virtual size_t content_length(void) const = 0;

    //! Returns the MIME type and subtype of the content if existing. Returns
    //! invalid type and subtype if no content exists.
    virtual mime content_type(void) const = 0;

    //! Gives its user access to a list of MIME types and subtype in the accept
    //! header. To get the first item, the user has to initialize the handle
    //! with a nullptr. The handle will point to the current item after each
    //! call. To get the next item just recall it. The function returns false,
    //! if the end of the list is reached and true in any other case. Till the
    //! end of the function is reached, the parameters are getting modified by
    //! the call. This functionality is mainly used by the demultiplexer, that
    //! tries to find the best request handler for the request.
    //!
    //! @code{.cpp}
    //! void* handle = nullptr;
    //! mime type;
    //! while (true == request->accept(handle, type)) {
    //!     // Do some fancy stuff.
    //! }
    //! @endcode
    virtual bool accept(void*& handle, mime& type) const = 0;

    //! Returns the expectation of the client. If the expectation is not
    //! implemented it returns UNKNOWN. Then the request processor will return
    //! status code 417 due to unmet expectation.
    virtual http_expectation expect(void) const = 0;

    //! Returns the content of the from field.
    virtual const char_t* from(void) const = 0;

    //! Returns the content of the referer field.
    virtual const char_t* referer(void) const = 0;

    //! Returns the content of the user agent field. Usually this is used to
    //! work around some idiosyncrasies of some specific clients to improve the
    //! result of the web service.
    virtual const char_t* user_agent(void) const = 0;
};

//! The request handler uses this interface to assemble the response.
class response_interface
{
public:
    virtual ~response_interface(void);

    //! Sets or overwrites a custom header. Every header field name, which is
    //! explicitly predefined by @ref page_requests gets rejected. Returns true,
    //! if the value was successfully set. A header field could get cleared by
    //! leaving the value empty. The header field will not be part of the
    //! response data in that case.
    virtual bool set_header(const char_t* const name,
                            const char_t* const value) = 0;

    //! Sets or overwrites the content and implicitly the content length of the
    //! response. The second parameter controls, whether a Content-MD5 field
    //! is generated or not.
    virtual void set_content(const buffer& content, const bool set_md5) = 0;

    //! Sets or overwrites the Content-Location header field.
    virtual void set_content_location(const char_t* const content_location) = 0;

    //! Sets or overwrites the Location header field.
    virtual void set_location(const char_t* const location) = 0;

    //! Sets or overwrites a retry timeout. The client should then repeat the
    //! request after it timed out. Setting 0 (the origin of the epoch time)
    //! clears that header field. Returns true, if the operation is accepted
    //! and false, if the parameter has an invalid value (e.g. negative time).
    virtual bool set_retry_after(const time_t retry_time) = 0;

    //! Sets the Server header field, which is, in fact, a fingerprint of the
    //! running server software. Usually this is used by the client to work
    //! around some idiosyncrasies. Per default this is not set for security
    //! reasons. So the user has to decide whether the benefits of setting a
    //! fingerprint outweights its costs (e.g. if the server software is public
    //! domain, it could be assumed, that attackers know about the
    //! implementations weaknesses). Consider that setting the server software
    //! name without a version may be a proper solution.
    virtual void set_server(const char_t* const fingerprint) = 0;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_REQUEST_HPP
