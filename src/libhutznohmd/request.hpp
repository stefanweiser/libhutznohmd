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
this is represented in the request interface. All the headers defined by the
current HTTP standard are listed in the following subpages.
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
in case of @c keep-alive the connection will be kept until the client closes the
connection or a predefined time duration elapsed without activity on the
connection. This time duration can be configured.

@todo [DOC] how the timeout can be configured

@subsubsection subsub_connction_example Example:

@code
Connection: keep-alive
@endcode

@subsubsection subsub_connction_default Default:

till @c HTTP/1.0:
@code
Connection: close
@endcode

since @c HTTP/1.1:
@code
Connection: keep-alive
@endcode

@subsubsection subsub_connction_implemented Implemented:

unimplemented

@subsection sub_content_length Content-Length

The header field of the @c Content-Length must be added by the client in
requests, if the request carries any payload data. The server will add this
header field, if the payload data of the response is not empty. It must then
contain the size of the payload in bytes. Thus it must be an unsigned integer.
The size is limited to \f$2^{31}-1\f$. In case of an overflow the request gets
rejected.

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

This header contains the timestamp, when the message was generated.

@todo [DOC] explain, when the date is used and how it can be configured

@subsubsection subsub_date_example Example:

@code
Date: Wed, 13 May 2014 22:10:48 GMT
@endcode

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
