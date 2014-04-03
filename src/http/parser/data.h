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

#ifndef __LIBREST_HTTP_PARSER_DATA_H__
#define __LIBREST_HTTP_PARSER_DATA_H__

#include <http/parser/httpscan.h>

#ifdef __cplusplus
#include <string>
#include <map>
#include <vector>

#include <librest.hpp>

extern "C"
{
#endif

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

void set_http_verb(httpscan_t * scanner, http_method method);
void set_http_version(httpscan_t * scanner, http_version version);
void set_status_code(httpscan_t * scanner, uint16_t code);
void append_to_url(httpscan_t * scanner, char token);
void append_to_reason_phrase(httpscan_t * scanner, char token);
void append_to_header_key(httpscan_t * scanner, char token);
void append_to_header_value(httpscan_t * scanner, char token);
void take_header(httpscan_t * scanner);

#ifdef __cplusplus
}

namespace rest
{

namespace http
{

class data
{
public:
    explicit data();

    void set_http_verb(const http_method & m);
    void set_http_version(const http_version & v);
    void set_status_code(uint16_t code);
    void append_to_url(char token);
    void append_to_reason_phrase(char token);
    void append_to_header_key(char token);
    void append_to_header_value(char token);
    void take_header();

    const http_method & method() const;
    const http_version & version() const;
    const std::string & url() const;
    const uint16_t & status_code() const;
    const std::string & reason_phrase() const;
    const std::string & header(const header_type & type) const;
    const std::string & custom_header(const std::string & key) const;
    const size_t & content_length() const;

private:
    const std::string empty_;
    std::string header_key_;
    std::string header_value_;

    http_method method_;
    http_version version_;
    std::string url_;
    uint16_t status_code_;
    std::string reason_phrase_;
    std::map<header_type, std::string> headers_;
    std::map<std::string, std::string> custom_headers_;

    size_t content_length_;
};

header_type header_string_to_enum(const std::string & s);

} // namespace http

} // namespace rest
#endif

#endif /* __LIBREST_HTTP_PARSER_DATA_H__ */
