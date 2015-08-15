/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

 * The libhutznohmd project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The libhutznohmd project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the libhutznohmd project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <cstring>
#include <limits>

#include "request.hpp"

namespace hutzn
{

request::request(const connection_pointer& connection)
    : lexer_(connection)
    , maximum_method_length_(0)
    , methods_(true)
    , maximum_version_length_(0)
    , versions_(true)
    , method_(http_verb::GET)
    , version_(http_version::HTTP_UNKNOWN)
    , header_fields_()
{
    // Filling methods and automatically calculate the maximum length.
    const std::vector<std::pair<const char* const, http_verb>> verbs = {
        std::make_pair("GET", http_verb::GET),
        std::make_pair("PUT", http_verb::PUT),
        std::make_pair("DELETE", http_verb::DELETE),
        std::make_pair("POST", http_verb::POST)};
    for (const std::pair<const char* const, http_verb>& pair : verbs) {
        methods_.insert(pair.first, pair.second);
        maximum_method_length_ =
            std::max(maximum_method_length_, ::strlen(pair.first));
    }

    // Filling versions and automatically calculate the maximum length.
    const std::vector<std::pair<const char* const, http_version>> methods = {
        std::make_pair("HTTP/1.0", http_version::HTTP_1_0),
        std::make_pair("HTTP/1.1", http_version::HTTP_1_1),
        std::make_pair("HTTP/2", http_version::HTTP_2)};
    for (const std::pair<const char* const, http_version>& pair : methods) {
        versions_.insert(pair.first, pair.second);
        maximum_version_length_ =
            std::max(maximum_version_length_, ::strlen(pair.first));
    }
}

bool request::fetch_header(void)
{
    bool result = false;

    int32_t ch = lexer_.get();
    if ((true == parse_method(ch)) && (true == parse_uri(ch)) &&
        (true == parse_version(ch))) {
        while (ch != -1) {
            if (true == parse_header(ch)) {
                ch = lexer_.get();

                if (true == is_newline(ch)) {
                    result = true;
                    break;
                }
            }
        }
    }

    return result;
}

bool request::parse_method(int32_t& ch)
{
    bool result = false;

    size_t method_begin = 0;
    while (ch != -1) {
        if (false == is_whitespace(ch)) {
            method_begin = lexer_.index() - 1;
            break;
        }

        ch = lexer_.get();
    }

    while (ch != -1) {
        if (true == is_whitespace(ch)) {
            const size_t method_length = lexer_.index() - 1 - method_begin;

            // Parsing the version will succeed, when the token is not too
            // long and the found method length is exactly the token length.
            if (method_length <= maximum_version_length_) {
                auto r =
                    methods_.find(lexer_.data(method_begin), method_length);
                if (r.used_size == method_length) {
                    method_ = r.value;
                    result = true;
                }
            }
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

bool request::parse_uri(int32_t& ch)
{
    bool result = false;

    while (ch != -1) {
        if (false == is_whitespace(ch)) {
            break;
        }

        ch = lexer_.get();
    }

    while (ch != -1) {
        if (true == is_whitespace(ch)) {
            result = true;
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

bool request::parse_version(int32_t& ch)
{
    bool result = false;

    size_t version_begin = 0;
    while (ch != -1) {
        if (false == is_whitespace(ch)) {
            version_begin = lexer_.index() - 1;
            break;
        }

        ch = lexer_.get();
    }

    while (ch != -1) {
        if (true == is_newline(ch)) {
            const size_t version_length = lexer_.index() - 1 - version_begin;

            // Parsing the version will succeed, when the token is not too
            // long and the found method length is exactly the token length.
            if (version_length <= maximum_version_length_) {
                auto r =
                    versions_.find(lexer_.data(version_begin), version_length);
                if (r.used_size == version_length) {
                    version_ = r.value;
                    result = true;
                }
            }
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

bool request::parse_header(int32_t& ch)
{
    bool result = false;

    size_t begin = lexer_.index() - 1;
    const char_t* key = lexer_.data(begin - 1);
    while (ch != -1) {
        if (false == is_key_value_seperator(ch)) {
            // Overwrite the seperator with null. The key is getting null
            // terminated by this.
            lexer_.data(lexer_.index() - 1)[0] = '\0';
            break;
        }

        ch = lexer_.get();
    }

    ch = lexer_.get();
    const char_t* value = lexer_.data(begin - 1);

    // The value of -1 signalizes end of file.
    while (ch != -1) {
        // Searching a newline character. This is the end of the header field.
        if (true == is_newline(ch)) {
            // Overwrite the newline with null. The value is getting null
            // terminated by this.
            lexer_.data(lexer_.index() - 1)[0] = '\0';
            header_fields_[key] = value;
            result = true;
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

http_verb request::method(void) const
{
    return method_;
}

const char_t* request::path(void) const
{
    return nullptr;
}

const char_t* request::host(void) const
{
    return nullptr;
}

const char_t* request::query(const char_t* const /*key*/) const
{
    return nullptr;
}

const char_t* request::fragment(void) const
{
    return nullptr;
}

http_version request::version(void) const
{
    return version_;
}

const char_t* request::header_value(const char_t* const name) const
{
    const char_t* result = nullptr;
    auto it = header_fields_.find(name);
    if (it != header_fields_.end()) {
        result = it->second;
    }
    return result;
}

bool request::keeps_connection(void) const
{
    return false;
}

time_t request::date(void) const
{
    return 0;
}

void* request::content(void) const
{
    return nullptr;
}

size_t request::content_length(void) const
{
    return 0;
}

mime request::content_type(void) const
{
    return mime(mime_type::INVALID, mime_subtype::INVALID);
}

bool request::accept(void*& /*handle*/, mime& /*type*/) const
{
    return false;
}

http_expectation request::expect(void) const
{
    return http_expectation::UNKNOWN;
}

const char_t* request::from(void) const
{
    return nullptr;
}

const char_t* request::referer(void) const
{
    return nullptr;
}

const char_t* request::user_agent(void) const
{
    return nullptr;
}

bool request::is_whitespace(const int32_t ch)
{
    // The character has to get converted into the more common data type.
    // Otherwise the comparison may offer some ambiguities (e.g. 1024 get to 0).
    // Doing this explicitly is more clear.
    return ((static_cast<int32_t>(' ') == ch) ||
            (static_cast<int32_t>('\t') == ch));
}

bool request::is_newline(const int32_t ch)
{
    // The character has to get converted into the more common data type.
    // Otherwise the comparison may offer some ambiguities (e.g. 1024 get to 0).
    // Doing this explicitly is more clear.
    return (static_cast<int32_t>('\n') == ch);
}

bool request::is_key_value_seperator(const int32_t ch)
{
    // The character has to get converted into the more common data type.
    // Otherwise the comparison may offer some ambiguities (e.g. 1024 get to 0).
    // Doing this explicitly is more clear.
    return (static_cast<int32_t>(':') == ch);
}

} // namespace hutzn
