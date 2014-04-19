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

#ifndef __LIBREST_HTTP_PARSER_HTTP_HPP__
#define __LIBREST_HTTP_PARSER_HTTP_HPP__

#include <functional>
#include <map>
#include <string>

#include <librest.hpp>

#include <http/parser/utility/pushbackstring.hpp>
#include <http/parser/utility/lexer.hpp>

enum class parser_state
{
    UNFINISHED = 0,
    SUCCEEDED = 1,
    ERROR = 2
};

typedef struct httpscan {
    lexer lexer_;
    parser_state state_;

    push_back_string<40> header_key_;
    push_back_string<1000> header_value_;
    rest::http::method method_;
    rest::http::version version_;
    push_back_string<1000> url_;
    uint16_t status_code_;
    push_back_string<100> reason_phrase_;
    std::map<std::string, std::string> headers_;
    size_t content_length_;
    time_t date_;
} httpscan_t;

time_t parse_timestamp(int & character, httpscan_t * scanner);
void http_parse(httpscan_t * scanner);
rest::http::header_type header_key_to_header_type(const std::string & s);

#endif // __LIBREST_HTTP_PARSER_HTTP_HPP__
