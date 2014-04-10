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

#include <cstring>
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

template<size_t size>
class push_back_string
{
public:
    explicit push_back_string()
        : current_length_(0)
        , dynamic_size_(0)
        , dynamic_buffer_(nullptr) {
    }

    ~push_back_string() {
        free(dynamic_buffer_);
    }

    push_back_string(const push_back_string &)
        : current_length_(0)
        , dynamic_size_(0)
        , dynamic_buffer_(nullptr) {
    }

    push_back_string & operator=(const push_back_string &) = delete;

    void push_back(const char c) {
        if (current_length_ < size) {
            constant_buffer_[current_length_++] = c;
        } else {
            if ((current_length_ + 1) >= dynamic_size_) {
                if (nullptr == dynamic_buffer_) {
                    dynamic_size_ = (2 * size) + 1;
                    dynamic_buffer_ = static_cast<char *>(malloc(dynamic_size_));
                    memcpy(dynamic_buffer_, constant_buffer_, size);
                } else {
                    dynamic_size_ += size;
                    dynamic_buffer_ = static_cast<char *>(realloc(dynamic_buffer_, dynamic_size_));
                }
            }
            dynamic_buffer_[current_length_++] = c;
        }
    }

    const char * c_str() const {
        if (nullptr == dynamic_buffer_) {
            constant_buffer_[current_length_] = '\0';
            return constant_buffer_;
        } else {
            dynamic_buffer_[current_length_] = '\0';
            return dynamic_buffer_;
        }
    }

    bool empty() const {
        return (current_length_ == 0);
    }

    void clear() {
        if (nullptr != dynamic_buffer_) {
            free(dynamic_buffer_);
            dynamic_size_ = 0;
        }
        current_length_ = 0;
    }

private:
    size_t current_length_;
    size_t dynamic_size_;
    mutable char constant_buffer_[size + 1];
    mutable char * dynamic_buffer_;
};

class anonymous_int_function
{
public:
    explicit anonymous_int_function(int (*functor)(void *), void * handle)
        : functor_(functor)
        , handle_(handle) {
    }

    int operator()() {
        return functor_(handle_);
    }

private:
    int (*functor_)(void *);
    void * handle_;
};

typedef struct httpscan {
    anonymous_int_function get_functor_;
    anonymous_int_function peek_functor_;
    char last_char_;
    lexer_state state_;

    push_back_string<40> header_key_;
    push_back_string<1000> header_value_;
    http_method method_;
    http_version version_;
    push_back_string<1000> url_;
    uint16_t status_code_;
    push_back_string<100> reason_phrase_;
    std::map<rest::http::header_type, std::string> headers_;
    std::map<std::string, std::string> custom_headers_;
    size_t content_length_;
} httpscan_t;

void http_parse(httpscan_t * scanner);
rest::http::header_type header_key_to_header_type(const std::string & s);

#endif // __LIBREST_HTTP_PARSER_HTTP_HPP__
