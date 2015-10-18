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

#include "request.hpp"

namespace hutzn
{

/*

NOTE: This definitions are getting used in later versions.

-# @subpage page_basic_http_support
-# @subpage page_encoding
-# @subpage page_auth
-# @subpage page_caching
-# @subpage page_cookies
-# @subpage page_byte_serving
-# @subpage page_conditional


@page page_basic_http_support Basic HTTP support


@page page_encoding Encoding

@section sec_encoding_request_and_response Request- and Response-Specific

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

@section sec_caching_request_and_response Request- and Response-Specific

@subsection sub_cache_control Cache-Control

<td>TBD</td>
<td><pre>Cache-Control: no-cache</pre></td>

@section sec_caching_response Response-Specific

@subsection sub_etag ETag

<td>Contains the current value of the entity tag.</td>
<td><pre>ETag: "0123456789abcdef"</pre></td>

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

request_interface::~request_interface(void)
{
}

response_interface::~response_interface(void)
{
}

} // namespace hutzn
