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
  namespace request {
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

    interface request_interface {
      +method(): http_verb
      +path(): string
      +version(): http_version
      +keeps_connection(): bool
      +date(): time
      +content(): buffer
      +content_length(): size
      +content_type(): string
      +accept(in/out handle: pointer): string
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
    http_version -- response_interface: < uses
    http_status_code -- response_interface: < uses
    request_interface <|-- request: implements
    response_interface <|-- response: implements
  }
}
@enduml

All the headers defined by the current HTTP standard are listed in the following
subpages.
-# @subpage page_basic_http_support
-# @subpage page_encoding
-# @subpage page_auth
-# @subpage page_caching
-# @subpage page_cookies
-# @subpage page_byte_serving
-# @subpage page_conditional

Every header field documentation is splitted into three parts:
- @a Description: What and how does it influence the system? What can you
expect? Informations on the content.
- @a Example: A common example on how to use it.
- @a Default: Which value the server supposes, if the header field is missing.
- @a Implemented: Since when is it implemented by the library.



@page page_basic_http_support Basic HTTP support

@section sec_basic_common Common

@subsection sub_connection Connection

Determines, what to do with the connection after the request is finished. Two
values are possible: @c close and @c keep-alive.

In case of @c close the connection will be closed after the response is sent and
in case of @c keep-alive the connection will be kept open until the client
closes the connection or a predefined time duration elapsed without activity on
the connection.

This time duration can be configured for the request processor in its factory
function @ref hutzn::demux::make_request_processor. It shall be deemed to be a
lower bound. It could take longer till the connection is effectively getting
closed. See @ref sec_lifetime_connection for more information.

@subsubsection subsub_connection_example Example:

@code
Connection: keep-alive
@endcode

@subsubsection subsub_connection_default Default:

till @c HTTP/1.0:
@code
Connection: close
@endcode

since @c HTTP/1.1:
@code
Connection: keep-alive
@endcode

@subsubsection subsub_connection_implemented Implemented:

unimplemented

@subsection sub_content_length Content-Length

The header field of the @c Content-Length must be added by the client in
requests, if the request carries any payload data. The server will add this
header field, if the payload data of the response is not empty. It must then
contain the size of the payload in bytes. Thus it must be an unsigned integer.
The size is limited to \f$2^{31}-1\f$. In case of an overflow the request
invalid and therefore rejected.

@subsubsection subsub_content_length_example Example:

@code
Content-Length: 123
@endcode

@subsubsection subsub_content_length_default Default:

@code
Content-Length: 0
@endcode

@subsubsection subsub_content_length_implemented Implemented:

unimplemented

@subsection sub_content_md5 Content-MD5

This header field is optional and carries a MD5 hash sum. This hash sum is used
to verify the content. If the header field is missing, the content is not
verified with MD5. Thus there is no default value.

@note This is no security feature, because of 2 reasons:
-# Collisions for MD5 could be found within reasonable time by recent hardware.
-# An attacker that is able to modify the header field or the content is
everytime also able to modify both.

@subsubsection subsub_content_md5_example Example:

@code
Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==
@endcode

@subsubsection subsub_content_md5_default Default:

@code
Content-MD5:
@endcode

@subsubsection subsub_content_md5_implemented Implemented:

unimplemented

@subsection sub_content_type Content-Type

A content type header defines how the application should interpret the content.
It consists of one MIME type.

@subsubsection subsub_content_type_example Example:

@code
Content-Type: application/xml
@endcode

@subsubsection subsub_content_type_default Default:

@code
Content-Type: text/plain
@endcode

@subsubsection subsub_content_type_implemented Implemented:

unimplemented

@subsection sub_date Date

This header contains the timestamp, when the message was generated. The request
processor will add its timestamp to the response.

@subsubsection subsub_date_example Example:

@code
Date: Wed, 13 May 2014 22:10:48 GMT
@endcode

@subsubsection subsub_date_default Default:

@code
Date: Thu, 01 Jan 1970 00:00:00 GMT
@endcode

@subsubsection subsub_date_implemented Implemented:

unimplemented

@section sec_basic_request Request-Specific

@subsection sub_accept Accept

<td>TBD</td>
<td><pre>Accept: text/plain</pre></td>

@subsection sub_expect Expect

<td>TBD</td>
<td><pre>Expect: 100-continue</pre></td>

@subsection sub_from From

Contains an e-mail address of a human, that controls the requesting user agent.

@subsubsection subsub_from_example Example:

@code
From: user@example.com
@endcode

@subsection sub_host Host

Refers to the requested host on the server in case of multiple domains hosted on
the server. Therefore it is part of the request URI. If the header is present,
the content may overwrite the content of the request URI.

@subsubsection subsub_host_example Example:

@code
Host: example.com
@endcode

@subsection sub_referer Referer

<td>TBD</td>
<td><pre>Referer: http://www.example.com/</pre></td>

@subsection sub_user_agent User-Agent

<td>TBD</td>
<td><pre>User-Agent: Googlebot/2.1 (+http://www.google.com/bot.html)</pre></td>

@section sec_basic_response Response-Specific

@subsection sub_allow Allow

<td>TBD</td>
<td><pre>Allow: GET, HEAD, PUT</pre></td>

@subsection sub_content_location Content-Location

<td>TBD</td>
<td><pre>Content-Location: /example</pre></td>

@subsection sub_etag ETag

<td>TBD</td>
<td><pre>ETag: "0123456789abcdef"</pre></td>

@subsection sub_location Location

<td>TBD</td>
<td><pre>Location: http://example.com/example</pre></td>

@subsection sub_retry_after Retry-After

<td>TBD</td>
<td><pre>Retry-After: 120</pre></td>

@subsection sub_server Server

<td>TBD</td>
<td><pre>Server: librest/x.y.z</pre></td>



@page page_encoding Encoding

@section sec_encoding_common Common

@subsection sub_content_encoding Content-Encoding

<td>TBD</td>
<td><pre>Content-Encoding: gzip</pre></td>

@subsection sub_content_language Content-Language

<td>TBD</td>
<td><pre>Content-Language: en-US</pre></td>

@section sec_encoding_request Request-Specific

@subsection sub_accept_charset Accept-Charset

<td>TBD</td>
<td><pre>Accept-Charset: utf-8</pre></td>

@subsection sub_accept_encoding Accept-Encoding

<td>TBD</td>
<td><pre>Accept-Encoding: gzip</pre></td>

@subsection sub_accept_language Accept-Language

<td>TBD</td>
<td><pre>Accept-Language: en-US</pre></td>



@page page_auth Authentification / Authorization

@section sec_auth_request Request-Specific

@subsection sub_authorization Authorization

<td>TBD</td>
<td><pre>Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==</pre></td>

@section sec_auth_response Response-Specific

@subsection sub_www_authenticate WWW-Authenticate

<td>TBD</td>
<td><pre>WWW-Authenticate: Basic</pre></td>



@page page_caching Caching

@section sec_caching_common Common

@subsection sub_cache_control Cache-Control

<td>TBD</td>
<td><pre>Cache-Control: no-cache</pre></td>

@section sec_caching_response Response-Specific

@subsection sub_expires Expires

<td>TBD</td>
<td><pre>Expires: Wed, 13 May 2014 22:10:48 GMT</pre></td>

@subsection sub_last_modified Last-Modified

<td>TBD</td>
<td><pre>Last-Modified: Wed, 13 May 2014 22:10:48 GMT</pre></td>

@subsection sub_content_pragma Pragma

<td>TBD</td>
<td><pre>Pragma: no-cache</pre></td>



@page page_cookies Cookies

@section sec_cookies_request Request-Specific

@subsection sub_cookie Cookie

<td>TBD</td>
<td><pre>Cookie: id=123</pre></td>

@section sec_cookies_response Response-Specific

@subsection sub_set_cookie Set-Cookie

<td>TBD</td>
<td><pre>Set-Cookie: id=123</pre></td>



@page page_byte_serving Byte-Serving

@section sec_byte_serving_request Request-Specific

@subsection sub_range Range

<td>TBD</td>
<td><pre>Range: bytes=0-2048</pre></td>

@section sec_byte_serving_response Response-Specific

@subsection sub_accept_ranges Accept-Ranges

<td>TBD</td>
<td><pre>Accept-Ranges: bytes</pre></td>

@subsection sub_content_range Content-Range

<td>TBD</td>
<td><pre>Content-Range: 0-2048/4096</pre></td>



@page page_conditional Conditional Requests

@section sec_conditional_request Request-Specific

@subsection sub_if_match If-Match

<td>TBD</td>
<td><pre>If-Match: "0123456789abcdef"</pre></td>

@subsection sub_if_modified_since If-Modified-Since

<td>TBD</td>
<td><pre>If-Modified-Since: Wed, 13 May 2014 22:10:48 GMT</pre></td>

@subsection sub_if_none_match If-None-Match

<td>TBD</td>
<td><pre>If-None-Match: "0123456789abcdef"</pre></td>

@subsection sub_if_range If-Range

<td>TBD</td>
<td><pre>If-Range: "0123456789abcdef"</pre></td>

@subsection sub_if_unmodified_since If-Unmodified-Since

<td>TBD</td>
<td><pre>If-Unmodified-Since: Wed, 13 May 2014 22:10:48 GMT</pre></td>

*/

//! This namespace contains all request related code and data. This includes a
//! request (readable) and response (writeable) interface for the server side.
namespace request
{

//! The MIME-Type consists of a group of subtypes.
enum class mime_type : uint8_t {
    //! The type is invalid. The implementation should reject the request or
    //! response.
    INVALID = 0,

    //! The character *. Used to catch all types or declaring that the subtype
    //! is not of interest.
    WILDCARD = 1,

    //! "application"
    APPLICATION = 2,

    //! "audio"
    AUDIO = 3,

    //! "image"
    IMAGE = 4,

    //! "text"
    TEXT = 5,

    //! "video"
    VIDEO = 6
};

//! The subtype of a MIME-Type defines the concrete format of the
//! representation.
enum class mime_subtype : uint16_t {
    //! The subtype is invalid. The implementation should reject the request or
    //! response.
    INVALID = 0,

    //! The character *. Used to catch all subtypes or declaring that the
    //! subtype is not of interest.
    WILDCARD = 1,

    //! "plain" means, that the content is not in any specific format.
    PLAIN = 2
};

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

    //! This version is currently unsupported and reserved for the next HTTP
    //! standard.
    HTTP_2_0 = 3
};

//! Every HTTP request has a specific verb. There are two properties to a subset
//! of methods.
//!
//! -# Idempotence: You can apply the identical request n times to the same URI
//!    without changing the result.
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
//! constexpr hutzn::request::http_status_code xy =
//!     static_cast<hutzn::request::http_status_code>(111);
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

class request_interface
{
public:
    virtual ~request_interface();

    //! Returns the HTTP verb used by the request (GET, PUT, DELETE or POST are
    //! allowed).
    virtual http_verb method() const = 0;

    //! The used URL without scheme, authorization, host, port, queries or
    //! fragment.
    virtual std::string path() const = 0;

    //! The host name used by the request client. This is currently just
    //! an information.
    virtual std::string host() const = 0;

    //! Returns a value of a key, that is in the query part of the URL.
    virtual std::string query(const std::string& key) const = 0;

    //! Returns the fragment of the URL.
    virtual std::string fragment() const = 0;

    //! Returns the used HTTP version. This influences server behaviour (e.g.
    //! connection duration).
    virtual http_version version() const = 0;

    //! Returns true, if the connection will be kept after the request is
    //! processed.
    virtual bool keeps_connection() const = 0;

    //! Returns the timestamp of the request (date header) as epoch time or 0
    //! if the header field does not exist.
    virtual time_t date() const = 0;

    //! Returns a pointer to the begin of the content in the buffer. This data
    //! buffer is @b not null-terminated and has the length of the
    //! content-length header field. If there is no content it returns nullptr.
    virtual void* content() const = 0;

    //! Returns the length of the buffer returned by
    //! @ref hutzn::request::request_interface::content(). If there is no
    //! content it returns 0.
    virtual size_t content_length() const = 0;

    //! Returns the MIME type and subtype of the content if existing. Returns
    //! invalid type and subtype if no content exists.
    virtual std::tuple<mime_type, mime_subtype> content_type() const = 0;

    //! Gives its user access to a list of MIME types and subtype in the accept
    //! header. To get the first item, you have to initialize the handle with a
    //! nullptr. The handle will point to the current item after each call. To
    //! get the next item just recall it. The function returns false, if the end
    //! of the list is reached and true in any other case. Till the end of the
    //! function is reached, the parameters are getting modified by the call.
    //!
    //! @code{.cpp}
    //! void* handle = nullptr;
    //! hutzn::request::mime_type type;
    //! hutzn::request::mime_subtype subtype;
    //! while (true == request->accept(handle, type, subtype)) {
    //!     // Do some fancy stuff.
    //! }
    //! @endcode
    virtual bool accept(void*& handle, mime_type& type,
                        mime_subtype& subtype) const = 0;
};

class response_interface
{
public:
    virtual ~response_interface();

    virtual bool set_status_code(const http_status_code& status_code) = 0;
    virtual bool set_version(const http_version& version) = 0;
    virtual bool set_header(const std::string& key,
                            const std::string& value) = 0;
    virtual bool set_content(const hutzn::buffer& content) = 0;
};

} // namespace request

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_REQUEST_HPP
