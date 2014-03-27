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

#include <cmath>
#include <cstdint>
#include <map>
#include <sstream>

#include "data.h"

namespace rest
{

namespace http
{

class lexer;
class data;

}

}

typedef struct httpscan {
    rest::http::lexer * lexer;
    rest::http::data * data;
} httpscan_t;

namespace rest
{

namespace http
{

namespace
{

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

} // namespace

data::data()
    : header_key_()
    , header_value_()
    , method_(METHOD_UNKNOWN)
    , version_(VERSION_UNKNOWN)
    , url_()
    , status_code_(0)
    , reason_phrase_()
    , headers_()
    , content_length_(0)
{
}

void data::set_http_verb(const http_method & m)
{
    method_ = m;
}

void data::set_http_version(const http_version & v)
{
    version_ = v;
}

void data::set_status_code(uint16_t factor, char token)
{
    uint16_t n = static_cast<uint16_t>((token - '0') * factor);
    status_code_ = static_cast<uint16_t>(status_code_ + n);
}

void data::append_to_url(char token)
{
    url_ += to_lower(token);
}

void data::append_to_reason_phrase(char token)
{
    reason_phrase_ += token;
}

void data::append_to_header_key(char token)
{
    header_key_ += to_lower(token);
}

void data::append_to_header_value(char token)
{
    header_value_ += token;
}

void data::take_header()
{
    header_type type = header_string_to_enum(header_key_);
    if (type == header_type::CONTENT_LENGTH) {
        content_length_ = lexical_cast<size_t>(header_value_);
    }

    headers_[header_key_] = header_value_;
    header_key_.clear();
    header_value_.clear();
}

const http_method & data::method() const
{
    return method_;
}

const http_version & data::version() const
{
    return version_;
}

const std::string & data::url() const
{
    return url_;
}

const uint16_t & data::status_code() const
{
    return status_code_;
}

const std::string & data::reason_phrase() const
{
    return reason_phrase_;
}

const std::map<std::string, std::string> & data::headers() const
{
    return headers_;
}

const size_t & data::content_length() const
{
    return content_length_;
}

header_type header_string_to_enum(const std::string & s)
{
    const static std::map<std::string, header_type> string_to_enum_map = {
        {"accept", header_type::ACCEPT},
        {"accept-charset", header_type::ACCEPT_CHARSET},
        {"accept-encoding", header_type::ACCEPT_ENCODING},
        {"accept-language", header_type::ACCEPT_LANGUAGE},
        {"accept-ranges", header_type::ACCEPT_RANGES},
        {"age", header_type::AGE},
        {"allow", header_type::ALLOW},
        {"authorization", header_type::AUTHORIZATION},
        {"cache-control", header_type::CACHE_CONTROL},
        {"connection", header_type::CONNECTION},
        {"content-encoding", header_type::CONTENT_ENCODING},
        {"content-language", header_type::CONTENT_LANGUAGE},
        {"content-length", header_type::CONTENT_LENGTH},
        {"content-location", header_type::CONTENT_LOCATION},
        {"content-md5", header_type::CONTENT_MD5},
        {"content-range", header_type::CONTENT_RANGE},
        {"content-type", header_type::CONTENT_TYPE},
        {"cookie", header_type::COOKIE},
        {"date", header_type::DATE},
        {"etag", header_type::ETAG},
        {"expect", header_type::EXPECT},
        {"expires", header_type::EXPIRES},
        {"from", header_type::FROM},
        {"host", header_type::HOST},
        {"if-match", header_type::IF_MATCH},
        {"if-modified-since", header_type::IF_MODIFIED_SINCE},
        {"if-none-match", header_type::IF_NONE_MATCH},
        {"if-range", header_type::IF_RANGE},
        {"if-unmodified-since", header_type::IF_UNMODIFIED_SINCE},
        {"last-modified", header_type::LAST_MODIFIED},
        {"location", header_type::LOCATION},
        {"max-forwards", header_type::MAX_FORWARDS},
        {"proxy-authenticate", header_type::PROXY_AUTHENTICATE},
        {"proxy-authorization", header_type::PROXY_AUTHORIZATION},
        {"range", header_type::RANGE},
        {"referer", header_type::REFERER},
        {"retry-after", header_type::RETRY_AFTER},
        {"server", header_type::SERVER},
        {"te", header_type::TE},
        {"user-agent", header_type::USER_AGENT},
        {"vary", header_type::VARY},
        {"www-authenticate", header_type::WWW_AUTHENTICATE}
    };

    auto it = string_to_enum_map.find(s);
    if (it != string_to_enum_map.end()) {
        return it->second;
    }
    return header_type::CUSTOM;
}

} // namespace http

} // namespace rest



void set_http_verb(httpscan_t * scanner, http_method method)
{
    scanner->data->set_http_verb(method);
}

void set_http_version(httpscan_t * scanner, http_version version)
{
    scanner->data->set_http_version(version);
}

void set_status_code(httpscan_t * scanner, uint16_t factor, char token)
{
    scanner->data->set_status_code(factor, token);
}

void append_to_url(httpscan_t * scanner, char token)
{
    scanner->data->append_to_url(token);
}

void append_to_reason_phrase(httpscan_t * scanner, char token)
{
    scanner->data->append_to_reason_phrase(token);
}

void append_to_header_key(httpscan_t * scanner, char token)
{
    scanner->data->append_to_header_key(token);
}

void append_to_header_value(httpscan_t * scanner, char token)
{
    scanner->data->append_to_header_value(token);
}

void take_header(httpscan_t * scanner)
{
    scanner->data->take_header();
}
