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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_HTTPFIRSTLINE_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_HTTPFIRSTLINE_HPP__

#include <cstddef>
#include <ctime>

#include <librest.hpp>

#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/pushbackstring.hpp>

namespace rest
{

namespace http
{

bool lex_request_url(int32_t & result, push_back_string<1000> & url, const lexer & l);
version lex_http_version(int32_t & character, const lexer & l);
uint16_t lex_status_code(int32_t & result, const lexer & l);
bool lex_reason_phrase(int32_t & character, push_back_string<100> & phrase, const lexer & l);

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_HTTPFIRSTLINE_HPP__
