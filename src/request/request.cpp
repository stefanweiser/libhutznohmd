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

#include <utility/trie.hpp>

#include "request.hpp"

namespace hutzn
{

namespace
{

static trie<http_verb> get_method_trie(size_t& max_size)
{
    trie<http_verb> result{true};

    // Filling methods and automatically calculate the maximum length.
    max_size = 0;
    const std::vector<std::pair<const char* const, http_verb>> verbs = {
        std::make_pair("GET", http_verb::GET),
        std::make_pair("PUT", http_verb::PUT),
        std::make_pair("DELETE", http_verb::DELETE),
        std::make_pair("POST", http_verb::POST)};
    for (const std::pair<const char* const, http_verb>& pair : verbs) {
        result.insert(pair.first, pair.second);
        max_size = std::max(max_size, ::strlen(pair.first));
    }

    return result;
}

static trie<http_version> get_version_trie(size_t& max_size)
{
    trie<http_version> result{true};

    // Filling versions and automatically calculate the maximum length.
    max_size = 0;
    const std::vector<std::pair<const char* const, http_version>> methods = {
        std::make_pair("HTTP/1.0", http_version::HTTP_1_0),
        std::make_pair("HTTP/1.1", http_version::HTTP_1_1),
        std::make_pair("HTTP/2", http_version::HTTP_2)};
    for (const std::pair<const char* const, http_version>& pair : methods) {
        result.insert(pair.first, pair.second);
        max_size = std::max(max_size, ::strlen(pair.first));
    }

    return result;
}

} // namespace

request::request(const connection_pointer& connection)
    : lexer_(connection)
    , method_(http_verb::GET)
    , path_(nullptr)
    , fragment_(nullptr)
    , version_(http_version::HTTP_UNKNOWN)
    , header_fields_()
    , query_entries_()
{
}

bool request::parse(void)
{
    bool result = false;

    int32_t ch = lexer_.get();
    if ((true == parse_method(ch)) && (true == parse_uri(ch)) &&
        (true == parse_version(ch))) {
        ch = lexer_.get();
        while (ch != -1) {
            if (true == is_newline(ch)) {
                result = true;
                break;
            }

            if (true == parse_header(ch)) {
                ch = lexer_.get();
            }
        }
    }

    return result;
}

bool request::parse_method(int32_t& ch)
{
    static size_t maximum_method_length = 0;
    static const trie<http_verb> methods =
        get_method_trie(maximum_method_length);

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
            if (method_length <= maximum_method_length) {
                auto r = methods.find(lexer_.data(method_begin), method_length);
                if (r.used_size() == method_length) {
                    method_ = r.value();
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

    const char_t* value = lexer_.data(lexer_.index() - 1);
    while (ch != -1) {
        if (true == is_whitespace(ch)) {
            // Overwrite the newline with null. The value is getting null
            // terminated by this.
            lexer_.data(lexer_.index() - 1)[0] = '\0';
            path_ = value;
            result = true;
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

bool request::parse_version(int32_t& ch)
{
    static size_t maximum_version_length = 0;
    static const trie<http_version> versions =
        get_version_trie(maximum_version_length);

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
            if (version_length <= maximum_version_length) {
                auto r =
                    versions.find(lexer_.data(version_begin), version_length);
                if (r.used_size() == version_length) {
                    version_ = r.value();
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

    const char_t* key = lexer_.data(lexer_.index() - 1);
    while (ch != -1) {
        if (true == is_key_value_seperator(ch)) {
            // Overwrite the seperator with null. The key is getting null
            // terminated by this.
            lexer_.data(lexer_.index() - 1)[0] = '\0';
            break;
        }

        ch = lexer_.get();
    }

    ch = lexer_.get();
    const char_t* value = lexer_.data(lexer_.index() - 1);

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
    return path_;
}

const char_t* request::host(void) const
{
    return nullptr;
}

const char_t* request::query(const char_t* const key) const
{
    const char_t* result = nullptr;
    auto it = query_entries_.find(key);
    if (it != query_entries_.end()) {
        result = it->second;
    }
    return result;
}

const char_t* request::fragment(void) const
{
    return fragment_;
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

bool request::enforced_null_terminated_less::operator()(
    const char_t* const lhs, const char_t* const rhs) const
{
    bool result;
    if (lhs == nullptr) {
        result = (rhs != nullptr);
    } else {
        if (rhs == nullptr) {
            result = false;
        } else {
            // Exceptional use of unbound function strcmp (breaks MISRA C++:2008
            // Rule 18-0-5), because neither lhs nor rhs is really unbound. Both
            // are pointing to null-terminated parts of the lexer's raw data.
            // The null-termination is ensured by the parsing algorithm.
            result = (strcmp(lhs, rhs) < 0);
        }
    }
    return result;
}

} // namespace hutzn
