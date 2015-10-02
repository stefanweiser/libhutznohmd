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

#include <request/mime_handler.hpp>
#include <request/timestamp.hpp>
#include <utility/parsing.hpp>
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
    const std::vector<std::pair<const char_t* const, http_verb>> verbs = {
        std::make_pair("GET", http_verb::GET),
        std::make_pair("PUT", http_verb::PUT),
        std::make_pair("DELETE", http_verb::DELETE),
        std::make_pair("POST", http_verb::POST)};
    for (const std::pair<const char_t* const, http_verb>& pair : verbs) {
        result.insert(pair.first, pair.second);
        max_size = std::max(max_size, ::strnlen(pair.first, 16));
    }

    return result;
}

static trie<http_version> get_version_trie(size_t& max_size)
{
    trie<http_version> result{true};

    // Filling versions and automatically calculate the maximum length.
    max_size = 0;
    const std::vector<std::pair<const char_t* const, http_version>> methods = {
        std::make_pair("HTTP/1.0", http_version::HTTP_1_0),
        std::make_pair("HTTP/1.1", http_version::HTTP_1_1),
        std::make_pair("HTTP/2", http_version::HTTP_2)};
    for (const std::pair<const char_t* const, http_version>& pair : methods) {
        result.insert(pair.first, pair.second);
        max_size = std::max(max_size, ::strnlen(pair.first, 16));
    }

    return result;
}

static trie<header_key> get_header_key_trie(size_t& max_size)
{
    trie<header_key> result{true};

    // Filling versions and automatically calculate the maximum length.
    max_size = 0;
    const std::vector<std::pair<const char_t* const, header_key>> header_keys =
        {std::make_pair("connection", header_key::CONNECTION),
         std::make_pair("content-length", header_key::CONTENT_LENGTH),
         std::make_pair("content-type", header_key::CONTENT_TYPE),
         std::make_pair("date", header_key::DATE),
         std::make_pair("from", header_key::FROM),
         std::make_pair("referer", header_key::REFERER),
         std::make_pair("user-agent", header_key::USER_AGENT)};
    for (const std::pair<const char_t* const, header_key>& pair : header_keys) {
        result.insert(pair.first, pair.second);
        max_size = std::max(max_size, ::strnlen(pair.first, 16));
    }

    return result;
}

} // namespace

request::request(const connection_pointer& connection)
    : lexer_(connection)
    , method_(http_verb::GET)
    , path_uri_()
    , version_(http_version::HTTP_UNKNOWN)
    , content_length_(0)
    , content_type_(mime_type::INVALID, mime_subtype::INVALID)
    , content_(nullptr)
    , is_keep_alive_set_(false)
    , date_(0)
    , expect_(http_expectation::UNKNOWN)
    , from_(nullptr)
    , referer_(nullptr)
    , user_agent_(nullptr)
    , header_fields_()
    , query_entries_()
{
}

bool request::parse(const mime_handler& handler)
{
    bool result = false;

    const bool fetch_result = lexer_.fetch_header();
    int32_t ch = lexer_.get();
    if ((true == fetch_result) && (true == parse_method(ch)) &&
        (true == parse_uri(ch)) && (true == parse_version(ch))) {
        ch = lexer_.get();
        while (ch >= 0) {
            if (true == is_newline(ch)) {
                result = true;
                break;
            }

            if (true == parse_header(handler, ch)) {
                ch = lexer_.get();
            }
        }
    }

    return result;
}

void request::fetch_content(void)
{
    const size_t length = content_length();
    if ((length > 0) && (true == lexer_.fetch_content(length))) {
        content_ = lexer_.content();
    }
}

http_verb request::method(void) const
{
    return method_;
}

const char_t* request::path(void) const
{
    return path_uri_.path();
}

const char_t* request::host(void) const
{
    return path_uri_.host();
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
    return path_uri_.fragment();
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
    return ((version() > http_version::HTTP_1_0) ||
            (true == is_keep_alive_set_));
}

time_t request::date(void) const
{
    return date_;
}

const void* request::content(void) const
{
    return content_;
}

size_t request::content_length(void) const
{
    return content_length_;
}

mime request::content_type(void) const
{
    return content_type_;
}

bool request::accept(void*& /*handle*/, mime& /*type*/) const
{
    return false;
}

http_expectation request::expect(void) const
{
    return expect_;
}

const char_t* request::from(void) const
{
    return from_;
}

const char_t* request::referer(void) const
{
    return referer_;
}

const char_t* request::user_agent(void) const
{
    return user_agent_;
}

bool request::parse_method(int32_t& ch)
{
    static size_t maximum_method_length = 0;
    static const trie<http_verb> methods =
        get_method_trie(maximum_method_length);

    bool result = false;

    size_t method_begin = 0;
    while (ch >= 0) {
        if (false == is_whitespace(ch)) {
            method_begin = lexer_.prev_index();
            break;
        }

        ch = lexer_.get();
    }

    while (ch >= 0) {
        if (true == is_whitespace(ch)) {
            const size_t method_length = lexer_.prev_index() - method_begin;

            // Parsing the version will succeed, when the token is not too
            // long and the found method length is exactly the token length.
            if (method_length <= maximum_method_length) {
                auto r = methods.find(lexer_.header_data(method_begin),
                                      method_length);
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

    while (ch >= 0) {
        if (false == is_whitespace(ch)) {
            break;
        }

        ch = lexer_.get();
    }

    char_t* value = lexer_.header_data(lexer_.prev_index());
    size_t length = 0;
    while (ch >= 0) {
        if (true == is_whitespace(ch)) {
            // Overwrite the newline with null. The value is getting null
            // terminated by this.
            lexer_.header_data(lexer_.prev_index())[0] = '\0';
            result = path_uri_.parse(value, length, true);
            break;
        }

        ch = lexer_.get();
        length++;
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
    while (ch >= 0) {
        if (false == is_whitespace(ch)) {
            version_begin = lexer_.prev_index();
            break;
        }

        ch = lexer_.get();
    }

    while (ch >= 0) {
        if (true == is_newline(ch)) {
            const size_t version_length = lexer_.prev_index() - version_begin;

            // Parsing the version will succeed, when the token is not too
            // long and the found method length is exactly the token length.
            if (version_length <= maximum_version_length) {
                auto r = versions.find(lexer_.header_data(version_begin),
                                       version_length);
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

bool request::parse_header(const mime_handler& handler, int32_t& ch)
{
    bool result = false;
    static size_t maximum_header_key_length = 0;
    static const trie<header_key> header_keys =
        get_header_key_trie(maximum_header_key_length);

    const size_t key_begin = lexer_.prev_index();
    size_t key_size = 0;
    const char_t* key = lexer_.header_data(key_begin);
    while (ch >= 0) {
        if (true == is_key_value_separator(ch)) {
            // Overwrite the separator with null. The key is getting null
            // terminated by this.
            const size_t key_end = lexer_.prev_index();
            lexer_.header_data(key_end)[0] = '\0';
            key_size = key_end - key_begin;
            break;
        }

        ch = lexer_.get();
    }

    trie_find_result<header_key> key_result = header_keys.find(key, key_size);
    header_key key_enum = header_key::CUSTOM;
    if (key_result.used_size() > 0) {
        key_enum = key_result.value();
    }

    ch = lexer_.get();
    const size_t value_begin = lexer_.prev_index();
    const char_t* value = lexer_.header_data(value_begin);

    // The value of -1 signalizes end of file.
    while (ch >= 0) {
        // Searching a newline character. This is the end of the header field.
        if (true == is_newline(ch)) {
            // Overwrite the newline with null. The value is getting null
            // terminated by this.
            const size_t value_end = lexer_.prev_index();
            lexer_.header_data(value_end)[0] = '\0';
            const size_t value_size = value_end - value_begin;
            add_header(handler, key_enum, key, value, value_size);
            result = true;
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

void request::add_header(const mime_handler& handler, header_key key,
                         const char_t* const key_string,
                         const char_t* value_string, size_t value_length)
{
    skip_whitespace(value_string, value_length);

    switch (key) {
    case header_key::CONNECTION:
        static constexpr char_t keep_alive_str[] = "keep-alive";
        static constexpr size_t keep_alive_size = sizeof(keep_alive_str);
        static_assert(keep_alive_size > 0,
                      "Size of string keep-alive must be greater than 0.");
        if (0 == ::strncasecmp(value_string, keep_alive_str, keep_alive_size)) {
            is_keep_alive_set_ = true;
        }
        break;

    case header_key::CONTENT_LENGTH: {
        const int32_t length =
            parse_unsigned_integer(value_string, value_length);
        if (length >= 0) {
            content_length_ = static_cast<size_t>(length);
        }
        break;
    }

    case header_key::CONTENT_TYPE:
        set_content_type(handler, value_string, value_length);
        break;

    case header_key::DATE:
        date_ = parse_timestamp(value_string, value_length);
        break;

    case header_key::EXPECT:
        static constexpr char_t continue_str[] = "100-continue";
        static constexpr size_t continue_size = sizeof(continue_str);
        static_assert(continue_size > 0,
                      "Size of string 100-continue must be greater than 0.");
        if (0 == ::strncasecmp(value_string, continue_str, continue_size)) {
            expect_ = http_expectation::CONTINUE;
        }
        break;

    case header_key::FROM:
        from_ = value_string;
        break;

    case header_key::REFERER:
        referer_ = value_string;
        break;

    case header_key::USER_AGENT:
        user_agent_ = value_string;
        break;

    case header_key::CUSTOM:
    default:
        header_fields_[key_string] = value_string;
        break;
    }
}

void request::set_content_type(const mime_handler& handler,
                               const char_t* value_string, size_t value_length)
{
    const char_t* type_begin = value_string;
    size_t type_size = 0;

    while ((type_size < value_length) && ('/' != (*value_string))) {
        value_string++;
        type_size++;
    }

    value_length -= type_size;
    if (value_length > 0) {
        value_string++;
        value_length--;
    }

    const char_t* subtype_begin = value_string;
    size_t subtype_size = 0;

    static const select_char_map whitespace_map =
        make_select_char_map(' ', '\t', '\n', '\r');

    while ((subtype_size < value_length) &&
           (false == whitespace_map[static_cast<uint8_t>(*value_string)])) {
        value_string++;
        subtype_size++;
    }

    mime_type type = handler.parse_type(type_begin, type_size);
    mime_subtype subtype = handler.parse_subtype(subtype_begin, subtype_size);
    content_type_ = mime(type, subtype);
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

bool request::is_key_value_separator(const int32_t ch)
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
