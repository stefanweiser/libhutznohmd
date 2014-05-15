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
@page header_fields Header Fields

@section sec_common_fields Common

@htmlonly
<table border="1" cellpadding="2px" cellspacing="0px">
    <tr>
        <th>Header name</th>
        <th>Description</th>
        <th>Example</th>
        <th>Feature</th>
    </tr>
    <tr>
        <td>Cache-Control</td>
        <td>TBD</td>
        <td><pre>Cache-Control: no-cache</pre></td>
        <td>Caching</td>
    </tr>
    <tr>
        <td>Connection</td>
        <td>TBD</td>
        <td><pre>Connection: keep-alive</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Content-Encoding</td>
        <td>TBD</td>
        <td><pre>Content-Encoding: gzip</pre></td>
        <td>Encoding</td>
    </tr>
    <tr>
        <td>Content-Language</td>
        <td>TBD</td>
        <td><pre>Content-Language: en-US</pre></td>
        <td>Encoding</td>
    </tr>
    <tr>
        <td>Content-Length</td>
        <td>TBD</td>
        <td><pre>Content-Length: 0</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Content-MD5</td>
        <td>TBD</td>
        <td><pre>Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Content-Type</td>
        <td>TBD</td>
        <td><pre>Content-Type: text/plain</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Date</td>
        <td>TBD</td>
        <td><pre>Date: Wed, 13 May 2014 22:10:48 GMT</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Pragma</td>
        <td>TBD</td>
        <td><pre>Pragma: no-cache</pre></td>
        <td>Basic</td>
    </tr>
</table>
@endhtmlonly

@section sec_request_fields Request Specific

@htmlonly
<table border="1" cellpadding="2px" cellspacing="0px">
    <tr>
        <th>Header name</th>
        <th>Description</th>
        <th>Example</th>
        <th>Feature</th>
    </tr>
    <tr>
        <td>Accept</td>
        <td>TBD</td>
        <td><pre>Accept: text/plain</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Accept-Charset</td>
        <td>TBD</td>
        <td><pre>Accept-Charset: utf-8</pre></td>
        <td>Encoding</td>
    </tr>
    <tr>
        <td>Accept-Encoding</td>
        <td>TBD</td>
        <td><pre>Accept-Encoding: gzip</pre></td>
        <td>Encoding</td>
    </tr>
    <tr>
        <td>Accept-Language</td>
        <td>TBD</td>
        <td><pre>Accept-Language: en-US</pre></td>
        <td>Encoding</td>
    </tr>
    <tr>
        <td>Authorization</td>
        <td>TBD</td>
        <td><pre>Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==</pre></td>
        <td>Auth</td>
    </tr>
    <tr>
        <td>Cookie</td>
        <td>TBD</td>
        <td><pre>Cookie: id=123</pre></td>
        <td>Cookies</td>
    </tr>
    <tr>
        <td>Expect</td>
        <td>TBD</td>
        <td><pre>Expect: 100-continue</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>From</td>
        <td>TBD</td>
        <td><pre>From: user@example.com</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Host</td>
        <td>TBD</td>
        <td><pre>Host: example.com</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>If-Match</td>
        <td>TBD</td>
        <td><pre>If-Match: "0123456789abcdef"</pre></td>
        <td>Conditional</td>
    </tr>
    <tr>
        <td>If-Modified-Since</td>
        <td>TBD</td>
        <td><pre>If-Modified-Since: Wed, 13 May 2014 22:10:48 GMT</pre></td>
        <td>Conditional</td>
    </tr>
    <tr>
        <td>If-None-Match</td>
        <td>TBD</td>
        <td><pre>If-None-Match: "0123456789abcdef"</pre></td>
        <td>Conditional</td>
    </tr>
    <tr>
        <td>If-Range</td>
        <td>TBD</td>
        <td><pre>If-Range: "0123456789abcdef"</pre></td>
        <td>Byte Serving and Conditional</td>
    </tr>
    <tr>
        <td>If-Unmodified-Since</td>
        <td>TBD</td>
        <td><pre>If-Unmodified-Since: Wed, 13 May 2014 22:10:48 GMT</pre></td>
        <td>Conditional</td>
    </tr>
    <tr>
        <td>Range</td>
        <td>TBD</td>
        <td><pre>Range: bytes=0-2048</pre></td>
        <td>Byte Serving</td>
    </tr>
    <tr>
        <td>Referer</td>
        <td>TBD</td>
        <td><pre>Referer: http://www.example.com/</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>User-Agent</td>
        <td>TBD</td>
        <td><pre>User-Agent: Googlebot/2.1 (+http://www.google.com/bot.html)</pre></td>
        <td>Basic</td>
    </tr>
</table>
@endhtmlonly

@section sec_response_fields Response Specific

@htmlonly
<table border="1" cellpadding="2px" cellspacing="0px">
    <tr>
        <th>Header name</th>
        <th>Description</th>
        <th>Example</th>
        <th>Feature</th>
    </tr>
    <tr>
        <td>Accept-Ranges</td>
        <td>TBD</td>
        <td><pre>Accept-Ranges: bytes</pre></td>
        <td>Byte Serving</td>
    </tr>
    <tr>
        <td>Allow</td>
        <td>TBD</td>
        <td><pre>Allow: GET, HEAD, PUT</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Content-Location</td>
        <td>TBD</td>
        <td><pre>Content-Location: /example</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Content-Range</td>
        <td>TBD</td>
        <td><pre>Content-Range: 0-2048/4096</pre></td>
        <td>Byte Serving</td>
    </tr>
    <tr>
        <td>ETag</td>
        <td>TBD</td>
        <td><pre>ETag: "0123456789abcdef"</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Expires</td>
        <td>TBD</td>
        <td><pre>Expires: Wed, 13 May 2014 22:10:48 GMT</pre></td>
        <td>Caching</td>
    </tr>
    <tr>
        <td>Last-Modified</td>
        <td>TBD</td>
        <td><pre>Last-Modified: Wed, 13 May 2014 22:10:48 GMT</pre></td>
        <td>Caching</td>
    </tr>
    <tr>
        <td>Location</td>
        <td>TBD</td>
        <td><pre>Location: http://example.com/example</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Retry-After</td>
        <td>TBD</td>
        <td><pre>Retry-After: 120</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Server</td>
        <td>TBD</td>
        <td><pre>Server: librest/x.y.z</pre></td>
        <td>Basic</td>
    </tr>
    <tr>
        <td>Set-Cookie</td>
        <td>TBD</td>
        <td><pre>Set-Cookie: id=123</pre></td>
        <td>Cookies</td>
    </tr>
    <tr>
        <td>WWW-Authenticate</td>
        <td>TBD</td>
        <td><pre>WWW-Authenticate: Basic</pre></td>
        <td>Auth</td>
    </tr>
</table>
@endhtmlonly

*/
