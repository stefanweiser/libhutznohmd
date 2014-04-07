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

#include <sstream>

#include "bisonwrapper.h"

char to_lower(const char c)
{
    if (c >= 'A' && c <= 'Z') {
        return (c | 0x60);
    }
    return c;
}

template <typename T>
T lexical_cast(const std::string & s)
{
    T result = T();
    std::istringstream input_stream;
    input_stream.str(s);
    input_stream >> result;
    return result;
}

rest::http::header_type header_string_to_enum(const std::string & s)
{
    const static std::map<std::string, rest::http::header_type> string_to_enum_map = {
        {"accept", rest::http::header_type::ACCEPT},
        {"accept-charset", rest::http::header_type::ACCEPT_CHARSET},
        {"accept-encoding", rest::http::header_type::ACCEPT_ENCODING},
        {"accept-language", rest::http::header_type::ACCEPT_LANGUAGE},
        {"accept-ranges", rest::http::header_type::ACCEPT_RANGES},
        {"age", rest::http::header_type::AGE},
        {"allow", rest::http::header_type::ALLOW},
        {"authorization", rest::http::header_type::AUTHORIZATION},
        {"cache-control", rest::http::header_type::CACHE_CONTROL},
        {"connection", rest::http::header_type::CONNECTION},
        {"content-encoding", rest::http::header_type::CONTENT_ENCODING},
        {"content-language", rest::http::header_type::CONTENT_LANGUAGE},
        {"content-length", rest::http::header_type::CONTENT_LENGTH},
        {"content-location", rest::http::header_type::CONTENT_LOCATION},
        {"content-md5", rest::http::header_type::CONTENT_MD5},
        {"content-range", rest::http::header_type::CONTENT_RANGE},
        {"content-type", rest::http::header_type::CONTENT_TYPE},
        {"cookie", rest::http::header_type::COOKIE},
        {"date", rest::http::header_type::DATE},
        {"etag", rest::http::header_type::ETAG},
        {"expect", rest::http::header_type::EXPECT},
        {"expires", rest::http::header_type::EXPIRES},
        {"from", rest::http::header_type::FROM},
        {"host", rest::http::header_type::HOST},
        {"if-match", rest::http::header_type::IF_MATCH},
        {"if-modified-since", rest::http::header_type::IF_MODIFIED_SINCE},
        {"if-none-match", rest::http::header_type::IF_NONE_MATCH},
        {"if-range", rest::http::header_type::IF_RANGE},
        {"if-unmodified-since", rest::http::header_type::IF_UNMODIFIED_SINCE},
        {"last-modified", rest::http::header_type::LAST_MODIFIED},
        {"location", rest::http::header_type::LOCATION},
        {"max-forwards", rest::http::header_type::MAX_FORWARDS},
        {"pragma", rest::http::header_type::PRAGMA},
        {"proxy-authenticate", rest::http::header_type::PROXY_AUTHENTICATE},
        {"proxy-authorization", rest::http::header_type::PROXY_AUTHORIZATION},
        {"range", rest::http::header_type::RANGE},
        {"referer", rest::http::header_type::REFERER},
        {"retry-after", rest::http::header_type::RETRY_AFTER},
        {"server", rest::http::header_type::SERVER},
        {"user-agent", rest::http::header_type::USER_AGENT},
        {"vary", rest::http::header_type::VARY},
        {"www-authenticate", rest::http::header_type::WWW_AUTHENTICATE}
    };

    auto it = string_to_enum_map.find(s);
    if (it != string_to_enum_map.end()) {
        return it->second;
    }
    return rest::http::header_type::CUSTOM;
}

int normalize_input(httpscan_t * scanner)
{
    if (lexer_state::FINISHED == scanner->state_) {
        return -1;
    }

    int result = scanner->get_functor_();
    if (result == '\r') {
        if (scanner->peek_functor_() == '\n') {
            scanner->get_functor_();
        }
        result = '\n';
    }

    if ((result == '\n') && (scanner->last_char_ != '\n')) {
        if ((scanner->peek_functor_() == ' ') || (scanner->peek_functor_() == '\t')) {
            scanner->get_functor_();
            result = ' ';
        }
    }

    scanner->last_char_ = static_cast<char>(result);
    return result;
}

int httplex(int * httplval, httpscan_t * scanner)
{
    int result = normalize_input(scanner);
    if (result < 0) {
        if (lexer_state::FINISHED != scanner->state_) {
            scanner->state_ = lexer_state::ERROR;
        }
        *httplval = -1;
        return -1;
    }

    switch (scanner->state_) {
    case lexer_state::START:
        if ((result == 'h') || (result == 'H')) {
            scanner->state_ = lexer_state::RESPONSE_VERSION;
        } else {
            scanner->state_ = lexer_state::REQUEST_METHOD;
        }
        break;

    case lexer_state::REQUEST_METHOD:
        break;

    case lexer_state::REQUEST_URL:
        break;

    case lexer_state::REQUEST_VERSION:
        break;

    case lexer_state::RESPONSE_VERSION:
        break;

    case lexer_state::RESPONSE_STATUS_CODE:
        break;

    case lexer_state::RESPONSE_REASON_PHRASE:
        break;

    case lexer_state::HEADER_KEY:
        break;

    case lexer_state::HEADER_VALUE:
        break;

    case lexer_state::FINISHED:
        break;

    case lexer_state::ERROR:
        break;

    default:
        break;
    }

    if (lexer_state::START == scanner->state_) {

    }

    *httplval = result;
    return *httplval;
}

void httperror(httpscan_t * /*scanner*/, const char * /*string*/)
{}

void set_http_verb(httpscan_t * scanner, http_method method)
{
    scanner->method_ = method;
}

void set_http_version(httpscan_t * scanner, http_version version)
{
    scanner->version_ = version;
}

void set_status_code(httpscan_t * scanner, uint16_t code)
{
    scanner->status_code_ = code;
}

void append_to_url(httpscan_t * scanner, char token)
{
    scanner->url_ += to_lower(token);
}

void append_to_reason_phrase(httpscan_t * scanner, char token)
{
    scanner->reason_phrase_ += token;
}

void append_to_header_key(httpscan_t * scanner, char token)
{
    scanner->header_key_ += to_lower(token);
}

void append_to_header_value(httpscan_t * scanner, char token)
{
    scanner->header_value_ += token;
}

void take_header(httpscan_t * scanner)
{
    rest::http::header_type type = header_string_to_enum(scanner->header_key_);
    if (type == rest::http::header_type::CONTENT_LENGTH) {
        scanner->content_length_ = lexical_cast<size_t>(scanner->header_value_);
    }

    if (type == rest::http::header_type::CUSTOM) {
        auto it = scanner->custom_headers_.find(scanner->header_key_);
        if (it == scanner->custom_headers_.end()) {
            scanner->custom_headers_[scanner->header_key_] = scanner->header_value_;
        } else {
            it->second += std::string(",") + scanner->header_value_;
        }
    } else {
        auto it = scanner->headers_.find(type);
        if (it == scanner->headers_.end()) {
            scanner->headers_[type] = scanner->header_value_;
        } else {
            it->second += std::string(",") + scanner->header_value_;
        }
    }

    scanner->header_key_.clear();
    scanner->header_value_.clear();
}

void http_finish(httpscan_t * scanner)
{
    scanner->state_ = lexer_state::FINISHED;
}
