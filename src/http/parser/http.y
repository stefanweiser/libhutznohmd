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

%{
#include <stdint.h>

#include <http/parser/bisonwrapper.h>
#include "http.h"
%}

%pure-parser
%define api.prefix {http}
%lex-param {httpscan_t * scanner}
%parse-param {httpscan_t * scanner}

%token TOKEN_METHOD
%token TOKEN_VERSION

%%

// HTTP structural rules ///////////////////////////////////////////////////////////////////////////
http:
  first_line nl headers nl { http_finish(scanner); }
| first_line nl nl         { http_finish(scanner); }
;

first_line:
  request_line | response_line
;

request_line:
  TOKEN_METHOD ws_1n url ws_1n version { set_http_verb(scanner, $1); }
;

response_line:
  TOKEN_VERSION ws_1n status_code ws reason_phrase { set_http_version(scanner, $1); }
;



// Request header and response header rules ////////////////////////////////////////////////////////
url:
  url url_char { append_to_url(scanner, $2); }
| url_char     { append_to_url(scanner, $1); }
;

version:
  http_1_0 { set_http_version(scanner, $1); }
| http_1_1 { set_http_version(scanner, $1); }
;

status_code:
  integer { set_status_code(scanner, $1); }
;

reason_phrase:
  reason_phrase reason_phrase_char { append_to_reason_phrase(scanner, $2); }
| reason_phrase_char               { append_to_reason_phrase(scanner, $1); }
;

reason_phrase_char:
  letter { $$ = $1; }
| digit  { $$ = $1; }
| ws     { $$ = $1; }



// Header rules ////////////////////////////////////////////////////////////////////////////////////
headers:
  headers header nl { take_header(scanner); }
| header nl         { take_header(scanner); }
;

header:
  header_key ':' header_value
| header_key ':'
;

header_key:
  header_key token_char { append_to_header_key(scanner, $2); }
| token_char            { append_to_header_key(scanner, $1); }
;

header_value:
  header_value header_content { append_to_header_value(scanner, $2); }
| header_content              { append_to_header_value(scanner, $1); }
;



// Simple type section /////////////////////////////////////////////////////////////////////////////
integer:
  integer digit { $$ = (10 * $1) + ($2 - 0x30); }
| digit         { $$ = ($1 - 0x30); }
;

ws_1n:
  ws_1n ws
| ws
;



// Words section ///////////////////////////////////////////////////////////////////////////////////
http_1_0: h t t p '/' '1' '.' '0' { $$ = VERSION_HTTP_1_0; };
http_1_1: h t t p '/' '1' '.' '1' { $$ = VERSION_HTTP_1_1; };



// Literal section /////////////////////////////////////////////////////////////////////////////////
header_content:
  '\x01' | '\x02' | '\x03' | '\x04' | '\x05' | '\x06' | '\x07' | '\x08' | '\x09' | '\x0B' | '\x0C'
| '\x0E' | '\x0F' | '\x10' | '\x11' | '\x12' | '\x13' | '\x14' | '\x15' | '\x16' | '\x17' | '\x18'
| '\x19' | '\x1A' | '\x1B' | '\x1C' | '\x1D' | '\x1E' | '\x1F' | '\x20' | '\x21' | '\x22' | '\x23'
| '\x24' | '\x25' | '\x26' | '\x27' | '\x28' | '\x29' | '\x2A' | '\x2B' | '\x2C' | '\x2D' | '\x2E'
| '\x2F' | '\x30' | '\x31' | '\x32' | '\x33' | '\x34' | '\x35' | '\x36' | '\x37' | '\x38' | '\x39'
| '\x3A' | '\x3B' | '\x3C' | '\x3D' | '\x3E' | '\x3F' | '\x40' | '\x41' | '\x42' | '\x43' | '\x44'
| '\x45' | '\x46' | '\x47' | '\x48' | '\x49' | '\x4A' | '\x4B' | '\x4C' | '\x4D' | '\x4E' | '\x4F'
| '\x50' | '\x51' | '\x52' | '\x53' | '\x54' | '\x55' | '\x56' | '\x57' | '\x58' | '\x59' | '\x5A'
| '\x5B' | '\x5C' | '\x5D' | '\x5E' | '\x5F' | '\x60' | '\x61' | '\x62' | '\x63' | '\x64' | '\x65'
| '\x66' | '\x67' | '\x68' | '\x69' | '\x6A' | '\x6B' | '\x6C' | '\x6D' | '\x6E' | '\x6F' | '\x70'
| '\x71' | '\x72' | '\x73' | '\x74' | '\x75' | '\x76' | '\x77' | '\x78' | '\x79' | '\x7A' | '\x7B'
| '\x7C' | '\x7D' | '\x7E' | '\x7F' | '\x80' | '\x81' | '\x82' | '\x83' | '\x84' | '\x85' | '\x86'
| '\x87' | '\x88' | '\x89' | '\x8A' | '\x8B' | '\x8C' | '\x8D' | '\x8E' | '\x8F' | '\x90' | '\x91'
| '\x92' | '\x93' | '\x94' | '\x95' | '\x96' | '\x97' | '\x98' | '\x99' | '\x9A' | '\x9B' | '\x9C'
| '\x9D' | '\x9E' | '\x9F' | '\xA0' | '\xA1' | '\xA2' | '\xA3' | '\xA4' | '\xA5' | '\xA6' | '\xA7'
| '\xA8' | '\xA9' | '\xAA' | '\xAB' | '\xAC' | '\xAD' | '\xAE' | '\xAF' | '\xB0' | '\xB1' | '\xB2'
| '\xB3' | '\xB4' | '\xB5' | '\xB6' | '\xB7' | '\xB8' | '\xB9' | '\xBA' | '\xBB' | '\xBC' | '\xBD'
| '\xBE' | '\xBF' | '\xC0' | '\xC1' | '\xC2' | '\xC3' | '\xC4' | '\xC5' | '\xC6' | '\xC7' | '\xC8'
| '\xC9' | '\xCA' | '\xCB' | '\xCC' | '\xCD' | '\xCE' | '\xCF' | '\xD0' | '\xD1' | '\xD2' | '\xD3'
| '\xD4' | '\xD5' | '\xD6' | '\xD7' | '\xD8' | '\xD9' | '\xDA' | '\xDB' | '\xDC' | '\xDD' | '\xDE'
| '\xDF' | '\xE0' | '\xE1' | '\xE2' | '\xE3' | '\xE4' | '\xE5' | '\xE6' | '\xE7' | '\xE8' | '\xE9'
| '\xEA' | '\xEB' | '\xEC' | '\xED' | '\xEE' | '\xEF' | '\xF0' | '\xF1' | '\xF2' | '\xF3' | '\xF4'
| '\xF5' | '\xF6' | '\xF7' | '\xF8' | '\xF9' | '\xFA' | '\xFB' | '\xFC' | '\xFD' | '\xFE' | '\xFF'
;

token_char:
  '\x21' | '\x23' | '\x24' | '\x25' | '\x26' | '\x27' | '\x2A' | '\x2B' | '\x2D' | '\x2E' | '\x30'
| '\x31' | '\x32' | '\x33' | '\x34' | '\x35' | '\x36' | '\x37' | '\x38' | '\x39' | '\x41' | '\x42'
| '\x43' | '\x44' | '\x45' | '\x46' | '\x47' | '\x48' | '\x49' | '\x4A' | '\x4B' | '\x4C' | '\x4D'
| '\x4E' | '\x4F' | '\x50' | '\x51' | '\x52' | '\x53' | '\x54' | '\x55' | '\x56' | '\x57' | '\x58'
| '\x59' | '\x5A' | '\x5E' | '\x5F' | '\x60' | '\x61' | '\x62' | '\x63' | '\x64' | '\x65' | '\x66'
| '\x67' | '\x68' | '\x69' | '\x6A' | '\x6B' | '\x6C' | '\x6D' | '\x6E' | '\x6F' | '\x70' | '\x71'
| '\x72' | '\x73' | '\x74' | '\x75' | '\x76' | '\x77' | '\x78' | '\x79' | '\x7A' | '\x7C' | '\x7E'
;

url_char:
  letter | digit | '-' | '.' | '_' | '~' | ':' | '/' | '?' | '#' | '[' | ']' | '@' | '!' | '$'
| '&' | '\'' | '(' | ')' | '*' | '+' | ',' | ';' | '='
;

letter:
  a | b | c | d | e | f | g | h | i | j | k | l | m
| n | o | p | q | r | s | t | u | v | w | x | y | z
;

digit:
  '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
;

ws: '\t' | ' ';
nl: '\n';
a: 'A' | 'a';
b: 'B' | 'b';
c: 'C' | 'c';
d: 'D' | 'd';
e: 'E' | 'e';
f: 'F' | 'f';
g: 'G' | 'g';
h: 'H' | 'h';
i: 'I' | 'i';
j: 'J' | 'j';
k: 'K' | 'k';
l: 'L' | 'l';
m: 'M' | 'm';
n: 'N' | 'n';
o: 'O' | 'o';
p: 'P' | 'p';
q: 'Q' | 'q';
r: 'R' | 'r';
s: 'S' | 's';
t: 'T' | 't';
u: 'U' | 'u';
v: 'V' | 'v';
w: 'W' | 'w';
x: 'X' | 'x';
y: 'Y' | 'y';
z: 'Z' | 'z';

%%

