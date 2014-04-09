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
%start start

%token TOKEN_METHOD
%token TOKEN_URL
%token TOKEN_VERSION
%token TOKEN_STATUS_CODE
%token TOKEN_REASON_PHRASE
%token TOKEN_CUSTOM_HEADER
%token TOKEN_CUSTOM_HEADER_VALUE

%%

start:
  first_line headers
;

first_line:
  request_line | response_line
;

request_line:
  TOKEN_METHOD TOKEN_URL TOKEN_VERSION
;

response_line:
  TOKEN_VERSION TOKEN_STATUS_CODE TOKEN_REASON_PHRASE
;

headers:
  headers custom_header
| %empty
;

custom_header:
  TOKEN_CUSTOM_HEADER TOKEN_CUSTOM_HEADER_VALUE
;

%%

