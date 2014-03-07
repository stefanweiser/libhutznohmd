%{
#include <stdint.h>

#include "http.h"
#include <http/parser/lexer.h>
%}

%pure-parser
%define api.prefix "http"
%lex-param {httpscan_t scanner}
%parse-param {void* scanner}

%%

http:           first-line
                newline
                headers
                newline { httpFinished(scanner); }

first-line:     request-line | response-line

request-line:   http-verb
                whitespace
                uri
                whitespace
                version

response-line:  version
                whitespace
                status-code
                whitespace
                reason-phrase

http-verb: options { setHttpVerb(scanner, METHOD_OPTIONS); }
           | get { setHttpVerb(scanner, METHOD_GET); }
           | head { setHttpVerb(scanner, METHOD_HEAD); }
           | post { setHttpVerb(scanner, METHOD_POST); }
           | put { setHttpVerb(scanner, METHOD_PUT); }
           | delete { setHttpVerb(scanner, METHOD_DELETE); }
           | trace { setHttpVerb(scanner, METHOD_TRACE); }
           | connect { setHttpVerb(scanner, METHOD_CONNECT); }

options: o p t i o n s
get: g e t
head: h e a d
post: p o s t
put: p u t
delete: d e l e t e
trace: t r a c e
connect: c o n n e c t

uri: uri uri-char { appendToUri(scanner); }
     | uri-char { appendToUri(scanner); }
uri-char: letter | digit | '-' | '.' | '_' | '~' | ':' | '/' | '?' | '#' | '[' | ']' | '@' | '!' | '$' | '&' | '\'' | '(' | ')' | '*' | '+' | ',' | ';' | '='

version: h t t p '/' '1' '.' '0' { setHttpVersion(scanner, VERSION_HTTP_1_0); }
         | h t t p '/' '1' '.' '1' { setHttpVersion(scanner, VERSION_HTTP_1_1); }

status-code: digit { setStatusCode(scanner, 100); }
             digit { setStatusCode(scanner, 10); }
             digit { setStatusCode(scanner, 1); }

reason-phrase: letter { appendToReasonPhrase(scanner); }
               reason-phrase-next
               | digit { appendToReasonPhrase(scanner); }
               reason-phrase-next
reason-phrase-next: reason-phrase-next letter { appendToReasonPhrase(scanner); }
                  | reason-phrase-next digit { appendToReasonPhrase(scanner); }
                  | reason-phrase-next ws { appendToReasonPhrase(scanner); }
                  |

headers: headers header |
header: header-key
        ':'
        header-value
        newline { takeHeader(scanner); }
header-key: header-key token-char { appendToHeaderKey(scanner); }
            | token-char { appendToHeaderKey(scanner); }
header-value: header-value header-content { appendToHeaderValue(scanner); }
              |
header-content: '\x01' | '\x02' | '\x03' | '\x04' | '\x05' | '\x06' | '\x07' | '\x08' | '\x09' | '\x0B' | '\x0C' | '\x0E' | '\x0F' | '\x10' | '\x11' | '\x12' | '\x13' | '\x14' | '\x15' | '\x16' | '\x17' | '\x18' | '\x19' | '\x1A' | '\x1B' | '\x1C' | '\x1D' | '\x1E' | '\x1F' | '\x20' | '\x21' | '\x22' | '\x23' | '\x24' | '\x25' | '\x26' | '\x27' | '\x28' | '\x29' | '\x2A' | '\x2B' | '\x2C' | '\x2D' | '\x2E' | '\x2F' | '\x30' | '\x31' | '\x32' | '\x33' | '\x34' | '\x35' | '\x36' | '\x37' | '\x38' | '\x39' | '\x3A' | '\x3B' | '\x3C' | '\x3D' | '\x3E' | '\x3F' | '\x40' | '\x41' | '\x42' | '\x43' | '\x44' | '\x45' | '\x46' | '\x47' | '\x48' | '\x49' | '\x4A' | '\x4B' | '\x4C' | '\x4D' | '\x4E' | '\x4F' | '\x50' | '\x51' | '\x52' | '\x53' | '\x54' | '\x55' | '\x56' | '\x57' | '\x58' | '\x59' | '\x5A' | '\x5B' | '\x5C' | '\x5D' | '\x5E' | '\x5F' | '\x60' | '\x61' | '\x62' | '\x63' | '\x64' | '\x65' | '\x66' | '\x67' | '\x68' | '\x69' | '\x6A' | '\x6B' | '\x6C' | '\x6D' | '\x6E' | '\x6F' | '\x70' | '\x71' | '\x72' | '\x73' | '\x74' | '\x75' | '\x76' | '\x77' | '\x78' | '\x79' | '\x7A' | '\x7B' | '\x7C' | '\x7D' | '\x7E' | '\x7F' | '\x80' | '\x81' | '\x82' | '\x83' | '\x84' | '\x85' | '\x86' | '\x87' | '\x88' | '\x89' | '\x8A' | '\x8B' | '\x8C' | '\x8D' | '\x8E' | '\x8F' | '\x90' | '\x91' | '\x92' | '\x93' | '\x94' | '\x95' | '\x96' | '\x97' | '\x98' | '\x99' | '\x9A' | '\x9B' | '\x9C' | '\x9D' | '\x9E' | '\x9F' | '\xA0' | '\xA1' | '\xA2' | '\xA3' | '\xA4' | '\xA5' | '\xA6' | '\xA7' | '\xA8' | '\xA9' | '\xAA' | '\xAB' | '\xAC' | '\xAD' | '\xAE' | '\xAF' | '\xB0' | '\xB1' | '\xB2' | '\xB3' | '\xB4' | '\xB5' | '\xB6' | '\xB7' | '\xB8' | '\xB9' | '\xBA' | '\xBB' | '\xBC' | '\xBD' | '\xBE' | '\xBF' | '\xC0' | '\xC1' | '\xC2' | '\xC3' | '\xC4' | '\xC5' | '\xC6' | '\xC7' | '\xC8' | '\xC9' | '\xCA' | '\xCB' | '\xCC' | '\xCD' | '\xCE' | '\xCF' | '\xD0' | '\xD1' | '\xD2' | '\xD3' | '\xD4' | '\xD5' | '\xD6' | '\xD7' | '\xD8' | '\xD9' | '\xDA' | '\xDB' | '\xDC' | '\xDD' | '\xDE' | '\xDF' | '\xE0' | '\xE1' | '\xE2' | '\xE3' | '\xE4' | '\xE5' | '\xE6' | '\xE7' | '\xE8' | '\xE9' | '\xEA' | '\xEB' | '\xEC' | '\xED' | '\xEE' | '\xEF' | '\xF0' | '\xF1' | '\xF2' | '\xF3' | '\xF4' | '\xF5' | '\xF6' | '\xF7' | '\xF8' | '\xF9' | '\xFA' | '\xFB' | '\xFC' | '\xFD' | '\xFE' | '\xFF'

whitespace: whitespace ws | ws
ws: '\t' | ' '

token-char: '\x21' | '\x23' | '\x24' | '\x25' | '\x26' | '\x27' | '\x2A' | '\x2B' | '\x2D' | '\x2E' | '\x30' | '\x31' | '\x32' | '\x33' | '\x34' | '\x35' | '\x36' | '\x37' | '\x38' | '\x39' | '\x41' | '\x42' | '\x43' | '\x44' | '\x45' | '\x46' | '\x47' | '\x48' | '\x49' | '\x4A' | '\x4B' | '\x4C' | '\x4D' | '\x4E' | '\x4F' | '\x50' | '\x51' | '\x52' | '\x53' | '\x54' | '\x55' | '\x56' | '\x57' | '\x58' | '\x59' | '\x5A' | '\x5E' | '\x5F' | '\x60' | '\x61' | '\x62' | '\x63' | '\x64' | '\x65' | '\x66' | '\x67' | '\x68' | '\x69' | '\x6A' | '\x6B' | '\x6C' | '\x6D' | '\x6E' | '\x6F' | '\x70' | '\x71' | '\x72' | '\x73' | '\x74' | '\x75' | '\x76' | '\x77' | '\x78' | '\x79' | '\x7A' | '\x7C' | '\x7E' | '\x7F'
newline: '\n' | '\r' '\n'
digit: '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
letter: a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r | s | t | u | v | w | x | y | z
a: 'A' | 'a'
b: 'B' | 'b'
c: 'C' | 'c'
d: 'D' | 'd'
e: 'E' | 'e'
f: 'F' | 'f'
g: 'G' | 'g'
h: 'H' | 'h'
i: 'I' | 'i'
j: 'J' | 'j'
k: 'K' | 'k'
l: 'L' | 'l'
m: 'M' | 'm'
n: 'N' | 'n'
o: 'O' | 'o'
p: 'P' | 'p'
q: 'Q' | 'q'
r: 'R' | 'r'
s: 'S' | 's'
t: 'T' | 't'
u: 'U' | 'u'
v: 'V' | 'v'
w: 'W' | 'w'
x: 'X' | 'x'
y: 'Y' | 'y'
z: 'Z' | 'z'

%%

/*
Request Header
==============

Accept
Accept-Charset
Accept-Encoding
Accept-Language
Authorization
Expect
From
Host
If-Match
If-Modified-Since
If-None-Match
If-Range
If-Unmodified-Since
Max-Forwards
Proxy-Authorization
Range
Referer
TE
User-Agent



Response Header
===============

Accept-Ranges
Age
ETag
Location
Proxy-Authenticate
Retry-After
Server
Vary
WWW-Authenticate



Entity Header
=============

Allow
Content-Encoding
Content-Language
Content-Length
Content-Location
Content-MD5
Content-Range
Content-Type
Expires
Last-Modified
*/