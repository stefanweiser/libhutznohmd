/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#include "memory_allocating_request.hpp"

#include <cassert>
#include <cstring>
#include <limits>

#include "request/base64.hpp"
#include "request/md5.hpp"
#include "request/mime_handler.hpp"
#include "request/timestamp.hpp"
#include "utility/parsing.hpp"
#include "utility/trie.hpp"

namespace hutzn
{

namespace
{

static const size_t trie_string_max_length = 16;

static trie<http_verb> get_method_trie(size_t& max_size)
{
    trie<http_verb> result{true};

    // filling methods and automatically calculate the maximum length
    max_size = 0;
    const std::vector<std::pair<const char_t* const, http_verb>> verbs = {
        std::make_pair("GET", http_verb::GET),
        std::make_pair("PUT", http_verb::PUT),
        std::make_pair("DELETE", http_verb::DELETE),
        std::make_pair("POST", http_verb::POST)};
    for (const std::pair<const char_t* const, http_verb>& pair : verbs) {
        result.insert(pair.first, pair.second);
        max_size =
            std::max(max_size, ::strnlen(pair.first, trie_string_max_length));
    }

    return result;
}

static trie<http_version> get_version_trie(size_t& max_size)
{
    trie<http_version> result{true};

    // filling versions and automatically calculate the maximum length
    max_size = 0;
    const std::vector<std::pair<const char_t* const, http_version>> methods = {
        std::make_pair("HTTP/1.0", http_version::HTTP_1_0),
        std::make_pair("HTTP/1.1", http_version::HTTP_1_1),
        std::make_pair("HTTP/2", http_version::HTTP_2)};
    for (const std::pair<const char_t* const, http_version>& pair : methods) {
        result.insert(pair.first, pair.second);
        max_size =
            std::max(max_size, ::strnlen(pair.first, trie_string_max_length));
    }

    return result;
}

static trie<header_key> get_header_key_trie(size_t& max_size)
{
    trie<header_key> result{true};

    // filling versions and automatically calculate the maximum length
    max_size = 0;
    const std::vector<std::pair<const char_t* const, header_key>> header_keys =
        {std::make_pair("connection", header_key::CONNECTION),
         std::make_pair("content-length", header_key::CONTENT_LENGTH),
         std::make_pair("content-md5", header_key::CONTENT_MD5),
         std::make_pair("content-type", header_key::CONTENT_TYPE),
         std::make_pair("date", header_key::DATE),
         std::make_pair("from", header_key::FROM),
         std::make_pair("referer", header_key::REFERER),
         std::make_pair("user-agent", header_key::USER_AGENT)};
    for (const std::pair<const char_t* const, header_key>& pair : header_keys) {
        result.insert(pair.first, pair.second);
        max_size =
            std::max(max_size, ::strnlen(pair.first, trie_string_max_length));
    }

    return result;
}

template <typename T>
bool parse_specific_value(const trie<T>& t, const size_t& max_length,
                          const char_t* const string, const size_t& length,
                          T& value)
{
    bool result = false;

    // parsing the value will succeed, when the token is not too long and the
    // found value length is exactly the token length
    if (length <= max_length) {
        auto r = t.find(string, length);
        if (r.used_size() == length) {
            value = r.value();
            result = true;
        }
    }

    return result;
}

} // namespace

memory_allocating_request::memory_allocating_request(
    const connection_ptr& connection)
    : lexer_(connection)
    , method_(http_verb::GET)
    , path_uri_()
    , version_(http_version::HTTP_UNKNOWN)
    , content_length_(0)
    , content_md5_(NULL)
    , content_md5_length_(0)
    , content_type_(mime_type::INVALID, mime_subtype::INVALID)
    , content_(NULL)
    , is_keep_alive_set_(false)
    , date_(0)
    , expect_(http_expectation::UNKNOWN)
    , from_(NULL)
    , referer_(NULL)
    , user_agent_(NULL)
    , header_fields_()
    , query_entries_()
{
}

bool memory_allocating_request::parse(const mime_handler& handler)
{
    bool result = false;

    const bool fetch_result = lexer_.fetch_header();
    int32_t ch = lexer_.get();
    if (fetch_result && parse_method(ch) && parse_uri(ch) &&
        parse_version(ch)) {
        ch = lexer_.get();
        while (ch >= 0) {
            if (is_newline(ch)) {
                result = true;
                break;
            }

            if (parse_header(handler, ch)) {
                ch = lexer_.get();
            }
        }
    }

    return result;
}

bool memory_allocating_request::fetch_content(void)
{
    bool result = true;

    const size_t length = content_length();
    if ((length > 0) && (lexer_.fetch_content(length))) {
        content_ = lexer_.content();
    }

    if (NULL != content_md5_) {
        md5_array md5_sum = calculate_md5(static_cast<const char_t*>(content_),
                                          content_length_);
        std::vector<uint8_t> expected_md5_sum =
            decode_base64(content_md5_, content_md5_length_);
        const bool md5_sum_valid = std::equal(md5_sum.begin(), md5_sum.end(),
                                              expected_md5_sum.begin());
        if (!md5_sum_valid) {
            content_ = NULL;
            result = false;
        }
    }

    return result;
}

http_verb memory_allocating_request::method(void) const
{
    return method_;
}

const char_t* memory_allocating_request::path(void) const
{
    return path_uri_.path();
}

const char_t* memory_allocating_request::host(void) const
{
    return path_uri_.host();
}

const char_t* memory_allocating_request::query(const char_t* const key) const
{
    const char_t* result = NULL;
    auto it = query_entries_.find(key);
    if (it != query_entries_.end()) {
        result = it->second;
    }
    return result;
}

const char_t* memory_allocating_request::fragment(void) const
{
    return path_uri_.fragment();
}

http_version memory_allocating_request::version(void) const
{
    return version_;
}

const char_t* memory_allocating_request::header_value(
    const char_t* const name) const
{
    const char_t* result = NULL;
    auto it = header_fields_.find(name);
    if (it != header_fields_.end()) {
        result = it->second;
    }
    return result;
}

bool memory_allocating_request::keeps_connection(void) const
{
    return (version() > http_version::HTTP_1_0) || is_keep_alive_set_;
}

time_t memory_allocating_request::date(void) const
{
    return date_;
}

const void* memory_allocating_request::content(void) const
{
    return content_;
}

size_t memory_allocating_request::content_length(void) const
{
    return content_length_;
}

mime memory_allocating_request::content_type(void) const
{
    return content_type_;
}

bool memory_allocating_request::accept(void*& /*handle*/, mime& /*type*/) const
{
    return false;
}

http_expectation memory_allocating_request::expect(void) const
{
    return expect_;
}

const char_t* memory_allocating_request::from(void) const
{
    return from_;
}

const char_t* memory_allocating_request::referer(void) const
{
    return referer_;
}

const char_t* memory_allocating_request::user_agent(void) const
{
    return user_agent_;
}

bool memory_allocating_request::parse_method(int32_t& ch)
{
    static size_t maximum_method_length = 0;
    static const trie<http_verb> methods =
        get_method_trie(maximum_method_length);

    bool result = false;

    size_t method_begin = 0;
    while (ch >= 0) {
        if (!is_whitespace(ch)) {
            method_begin = lexer_.prev_index();
            break;
        }

        ch = lexer_.get();
    }

    while (ch >= 0) {
        if (is_whitespace(ch)) {
            const size_t method_length = lexer_.prev_index() - method_begin;

            result = parse_specific_value(methods, maximum_method_length,
                                          lexer_.header_data(method_begin),
                                          method_length, method_);
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

bool memory_allocating_request::parse_uri(int32_t& ch)
{
    bool result = false;

    while (ch >= 0) {
        if (!is_whitespace(ch)) {
            break;
        }

        ch = lexer_.get();
    }

    char_t* source = lexer_.header_data(lexer_.prev_index());
    size_t source_length = 0;
    while (ch >= 0) {
        if (is_whitespace(ch)) {
            char_t* destination = lexer_.header_data(0);
            size_t destination_length = lexer_.prev_index();
            result = path_uri_.parse(source, source_length, destination,
                                     destination_length, true);
            break;
        }

        ch = lexer_.get();
        source_length++;
    }

    return result;
}

bool memory_allocating_request::parse_version(int32_t& ch)
{
    static size_t maximum_version_length = 0;
    static const trie<http_version> versions =
        get_version_trie(maximum_version_length);

    bool result = false;

    size_t version_begin = 0;
    while (ch >= 0) {
        if (!is_whitespace(ch)) {
            version_begin = lexer_.prev_index();
            break;
        }

        ch = lexer_.get();
    }

    while (ch >= 0) {
        if (is_newline(ch)) {
            const size_t version_length = lexer_.prev_index() - version_begin;

            result = parse_specific_value(versions, maximum_version_length,
                                          lexer_.header_data(version_begin),
                                          version_length, version_);
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

bool memory_allocating_request::parse_header(const mime_handler& handler,
                                             int32_t& ch)
{
    bool result = false;
    static size_t maximum_header_key_length = 0;
    static const trie<header_key> header_keys =
        get_header_key_trie(maximum_header_key_length);

    const size_t key_begin = lexer_.prev_index();
    size_t key_size = 0;
    const char_t* key = lexer_.header_data(key_begin);
    while (ch >= 0) {
        if (is_key_value_separator(ch)) {
            // overwrite the separator with null will null terminate the key
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

    // the value of -1 signalizes end of file
    while (ch >= 0) {
        // finding a newline character marks the end of the header field
        if (is_newline(ch)) {
            // overwrite the newline with null will null terminate the value
            const size_t value_end = lexer_.prev_index();
            lexer_.header_data(value_end)[0] = '\0';
            const size_t value_size = value_end - value_begin;
            set_header(handler, key_enum, key, value, value_size);
            result = true;
            break;
        }

        ch = lexer_.get();
    }

    return result;
}

void memory_allocating_request::set_header(const mime_handler& handler,
                                           header_key key,
                                           const char_t* const key_string,
                                           const char_t* value_string,
                                           size_t value_length)
{
    using set_header_functor =
        void (memory_allocating_request::*)(const mime_handler&, const char_t*,
                                            size_t);
    using header_fn_array =
        std::array<set_header_functor, static_cast<size_t>(header_key::SIZE)>;
    static const header_fn_array set_header_fns = {
        {&memory_allocating_request::set_connection,
         &memory_allocating_request::set_content_length,
         &memory_allocating_request::set_content_md5,
         &memory_allocating_request::set_content_type,
         &memory_allocating_request::set_date,
         &memory_allocating_request::set_expect,
         &memory_allocating_request::set_from,
         &memory_allocating_request::set_referer,
         &memory_allocating_request::set_user_agent}};
    skip_whitespace(value_string, value_length);

    if ((key > header_key::CUSTOM) && (key < header_key::SIZE)) {
        const set_header_functor fn = set_header_fns[static_cast<size_t>(key)];
        (this->*fn)(handler, value_string, value_length);
    } else {
        header_fields_[key_string] = value_string;
    }
}

void memory_allocating_request::set_connection(const mime_handler&,
                                               const char_t* value_string,
                                               size_t value_length)
{
    static const char_t keep_alive_str[] = "keep-alive";
    static const size_t keep_alive_size = sizeof(keep_alive_str);
    static_assert(keep_alive_size > 0,
                  "size of string keep-alive must be greater than 0");
    if ((keep_alive_size <= (value_length + 1)) &&
        (0 == ::strncasecmp(value_string, keep_alive_str, keep_alive_size))) {
        is_keep_alive_set_ = true;
    }
}

void memory_allocating_request::set_content_length(const mime_handler&,
                                                   const char_t* value_string,
                                                   size_t value_length)
{
    const int64_t length =
        parse_unsigned_integer<int64_t>(value_string, value_length);
    if (length >= 0) {
        content_length_ = static_cast<size_t>(length);
    }
}

void memory_allocating_request::set_content_md5(const mime_handler&,
                                                const char_t* value_string,
                                                size_t value_length)
{
    content_md5_ = value_string;
    content_md5_length_ = value_length;
}

void memory_allocating_request::set_content_type(const mime_handler& handler,
                                                 const char_t* value_string,
                                                 size_t value_length)
{
    content_type_ = handler.parse(value_string, value_length);
}

void memory_allocating_request::set_date(const mime_handler&,
                                         const char_t* value_string,
                                         size_t value_length)
{
    date_ = parse_timestamp(value_string, value_length);
}

void memory_allocating_request::set_expect(const mime_handler&,
                                           const char_t* value_string, size_t)
{
    static const char_t continue_str[] = "100-continue";
    static const size_t continue_size = sizeof(continue_str);
    static_assert(continue_size > 0,
                  "size of string 100-continue must be greater than 0");
    if (0 == ::strncasecmp(value_string, continue_str, continue_size)) {
        expect_ = http_expectation::CONTINUE;
    }
}

void memory_allocating_request::set_from(const mime_handler&,
                                         const char_t* value_string, size_t)
{
    from_ = value_string;
}

void memory_allocating_request::set_referer(const mime_handler&,
                                            const char_t* value_string, size_t)
{
    referer_ = value_string;
}

void memory_allocating_request::set_user_agent(const mime_handler&,
                                               const char_t* value_string,
                                               size_t)
{
    user_agent_ = value_string;
}

bool memory_allocating_request::is_whitespace(const int32_t ch)
{
    // the character has to get converted into the more common data type
    // otherwise the comparison may offer some ambiguities (e.g. 1024 get to 0)
    // doing this explicitly is more clear
    return (static_cast<int32_t>(' ') == ch) ||
           (static_cast<int32_t>('\t') == ch);
}

bool memory_allocating_request::is_newline(const int32_t ch)
{
    // the character has to get converted into the more common data type
    // otherwise the comparison may offer some ambiguities (e.g. 1024 get to 0)
    // doing this explicitly is more clear
    return static_cast<int32_t>('\n') == ch;
}

bool memory_allocating_request::is_key_value_separator(const int32_t ch)
{
    // the character has to get converted into the more common data type
    // otherwise the comparison may offer some ambiguities (e.g. 1024 get to 0)
    // doing this explicitly is more clear
    return static_cast<int32_t>(':') == ch;
}

bool memory_allocating_request::enforced_null_terminated_less::operator()(
    const char_t* const lhs, const char_t* const rhs) const
{
    bool result;
    if (lhs == NULL) {
        result = (rhs != NULL);
    } else {
        if (rhs == NULL) {
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
