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

typedef enum {
    METHOD_UNKNOWN = 0,
    METHOD_HEAD = 1,
    METHOD_GET = 2,
    METHOD_PUT = 3,
    METHOD_DELETE = 4,
    METHOD_POST = 5,
    METHOD_TRACE = 6,
    METHOD_OPTIONS = 7,
    METHOD_CONNECT = 8
}
http_method;

typedef enum {
    VERSION_UNKNOWN = 0,
    VERSION_HTTP_1_0 = 1,
    VERSION_HTTP_1_1 = 2
} http_version;

typedef enum {
    HEADER_CUSTOM = 0,
    HEADER_ACCEPT = 1,
    HEADER_ACCEPT_CHARSET = 2,
    HEADER_ACCEPT_ENCODING = 3,
    HEADER_ACCEPT_LANGUAGE = 4,
    HEADER_ACCEPT_RANGES = 5,
    HEADER_AGE = 6,
    HEADER_ALLOW = 7,
    HEADER_AUTHORIZATION = 8,
    HEADER_CACHE_CONTROL = 9,
    HEADER_CONNECTION = 10,
    HEADER_CONTENT_ENCODING = 11,
    HEADER_CONTENT_LANGUAGE = 12,
    HEADER_CONTENT_LENGTH = 13,
    HEADER_CONTENT_LOCATION = 14,
    HEADER_CONTENT_MD5 = 15,
    HEADER_CONTENT_RANGE = 16,
    HEADER_CONTENT_TYPE = 17,
    HEADER_COOKIE = 18,
    HEADER_DATE = 19,
    HEADER_ETAG = 20,
    HEADER_EXPECT = 21,
    HEADER_EXPIRES = 22,
    HEADER_FROM = 23,
    HEADER_HOST = 24,
    HEADER_IF_MATCH = 25,
    HEADER_IF_MODIFIED_SINCE = 26,
    HEADER_IF_NONE_MATCH = 27,
    HEADER_IF_RANGE = 28,
    HEADER_IF_UNMODIFIED_SINCE = 29,
    HEADER_LAST_MODIFIED = 30,
    HEADER_LOCATION = 31,
    HEADER_MAX_FORWARDS = 32,
    HEADER_PRAGMA = 33,
    HEADER_PROXY_AUTHENTICATE = 34,
    HEADER_PROXY_AUTHORIZATION = 35,
    HEADER_RANGE = 36,
    HEADER_REFERER = 37,
    HEADER_RETRY_AFTER = 38,
    HEADER_SERVER = 39,
    HEADER_USER_AGENT = 40,
    HEADER_VARY = 41,
    HEADER_WWW_AUTHENTICATE = 42
} header_type;

enum class lexer_state
{
    START = 0,

    REQUEST_METHOD = 1,
    REQUEST_URL = 2,
    REQUEST_VERSION = 3,

    RESPONSE_VERSION = 4,
    RESPONSE_STATUS_CODE = 5,
    RESPONSE_REASON_PHRASE = 6,

    HEADER = 7,

    FINISHED = 8,
    ERROR = 9
};

typedef struct httpscan {
    std::function<int()> get_functor_;
    std::function<int()> peek_functor_;
    char last_char_;
    lexer_state state_;

    std::string header_key_;
    std::string header_value_;
    http_method method_;
    http_version version_;
    std::string url_;
    uint16_t status_code_;
    std::string reason_phrase_;
    std::map<rest::http::header_type, std::string> headers_;
    std::map<std::string, std::string> custom_headers_;
    size_t content_length_;
} httpscan_t;

void http_parse(httpscan_t * scanner);
rest::http::header_type header_string_to_enum(const std::string & s);

#endif // __LIBREST_HTTP_PARSER_HTTP_HPP__
