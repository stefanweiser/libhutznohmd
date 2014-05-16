/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#error "Do not include this pure documentation file."

/*!
@page page_basic_http_support Basic HTTP support

@section sec_basic_common Common

@subsection sub_connection Connection

Determines, what to do with the connection after the request is finished. Two values
are possible: @c close and @c keep-alive.

In case of @c close the connection will be closed after the response is sent and in case of
@c keep-alive the connection will be kept until the client closes the connection or a predefined
time duration elapsed without activity on the connection. This time duration can be configured.
@todo How it can be configured.

@a Example:
@code
Connection: keep-alive
@endcode

@a Default:

@code
Connection: close (HTTP/1.0)
Connection: keep-alive (HTTP/1.1)
@endcode

@subsection sub_content_length Content-Length

<td>TBD</td>
<td><pre>Content-Length: 0</pre></td>

@subsection sub_content_md5 Content-MD5

<td>TBD</td>
<td><pre>Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==</pre></td>

@subsection sub_content_type Content-Type

<td>TBD</td>
<td><pre>Content-Type: text/plain</pre></td>

@subsection sub_content_date Date

<td>TBD</td>
<td><pre>Date: Wed, 13 May 2014 22:10:48 GMT</pre></td>

@subsection sub_content_pragma Pragma

<td>TBD</td>
<td><pre>Pragma: no-cache</pre></td>

@section sec_basic_request Request-Specific

@subsection sub_accept Accept

<td>TBD</td>
<td><pre>Accept: text/plain</pre></td>

@subsection sub_expect Expect

<td>TBD</td>
<td><pre>Expect: 100-continue</pre></td>

@subsection sub_from From

<td>TBD</td>
<td><pre>From: user@example.com</pre></td>

@subsection sub_host Host

<td>TBD</td>
<td><pre>Host: example.com</pre></td>

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

*/
