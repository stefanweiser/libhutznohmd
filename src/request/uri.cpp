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

#include <utility/character_validation.hpp>
#include <utility/parsing.hpp>
#include <utility/select_char_map.hpp>
#include <utility/trie.hpp>

#include "uri.hpp"

namespace hutzn
{

namespace
{

//! Makes and returns a newly created scheme trie.
static trie<std::tuple<uri_scheme, uint16_t>> make_scheme_trie()
{
    using value_type = std::tuple<uri_scheme, uint16_t>;
    trie<value_type> t{true};
    t.insert("http", value_type(uri_scheme::HTTP, 80));
    t.insert("https", value_type(uri_scheme::HTTPS, 443));
    return t;
}

inline bool convert_char(char_t*& data, size_t& head, size_t& tail)
{
    bool is_error;
    uint8_t a = static_cast<uint8_t>(from_hex(data[head + 1]));
    uint8_t b = static_cast<uint8_t>(from_hex(data[head + 2]));
    if ((a < 16) && (b < 16)) {
        data[tail] = static_cast<char_t>(static_cast<uint8_t>((a << 4) + b));
        head += 3;
        tail++;
        is_error = false;
    } else {
        tail = 0;
        is_error = true;
    }
    return is_error;
}

inline void skip_optional_slashes(char_t*& raw, size_t& remaining)
{
    if ((remaining >= 2) && ('/' == raw[0]) && ('/' == raw[1])) {
        remaining -= 2;
        raw += 2;
    }
}

//! Parses maximal remaining characters of data and stops, when one of the
//! selected characters is discovered.
template <typename... tn>
size_t parse_uri_word(char_t*& data, size_t& remaining,
                      const tn... select_chars)
{
    static const select_char_map map = make_select_char_map(select_chars...);

    size_t head = 0;
    size_t tail = 0;
    bool stop = false;
    while (!stop) {
        const char_t ch = data[head];
        if ((map[static_cast<uint8_t>(ch)]) || (head >= remaining)) {
            stop = true;
        } else if ('%' == ch) {
            if ((remaining - head) > 2) {
                stop = convert_char(data, head, tail);
            } else {
                tail = 0;
                stop = true;
            }
        } else {
            data[tail] = data[head];
            head++;
            tail++;
        }
    }

    remaining -= head;
    data += head;
    return tail;
}

template <typename... tn>
char_t* parse_optional_positional_token(
    char_t*& data, size_t& remaining, char_t& last_char, size_t& result_size,
    const char_t conditional_start_character, const tn... select_chars)
{
    char_t* result = nullptr;
    if ((remaining > 0) && (last_char == conditional_start_character)) {
        remaining--;
        data++;

        char_t* original_data = data;
        size_t length = parse_uri_word(data, remaining, select_chars...);
        last_char = *data;
        original_data[length] = '\0';
        result = original_data;
        result_size = length;
    }
    return result;
}

} // namespace

uri::uri(void)
    : already_called_(false)
    , scheme_(uri_scheme::UNKNOWN)
    , userinfo_(nullptr)
    , host_(nullptr)
    , port_(0)
    , path_(nullptr)
    , query_(nullptr)
    , fragment_(nullptr)
{
}

//! @todo seperate http and mailto uri parser to get rid of skip_scheme param.
bool uri::parse(char_t*& raw, size_t& remaining, bool skip_scheme)
{
    first_pass_data data;
    const bool passed_first_pass =
        parse_1st_pass(raw, remaining, data, skip_scheme);
    bool result = passed_first_pass;
    if (passed_first_pass) {
        // This implementation supports both: URIs with and without scheme or
        // authority. Though it is not strictly conforming with RFC 3986, HTTP
        // specifies request URIs without scheme and authority.
        if ((data.scheme_size > 0) &&
            (!parse_scheme(data.scheme, data.scheme_size))) {
            result = false;
        }
        if ((data.authority_size > 0) &&
            (!parse_authority(data.authority, data.authority_size))) {
            result = false;
        }
        if (data.path != nullptr) {
            path_ = data.path;
        }
        if (data.query != nullptr) {
            query_ = data.query;
        }
        if (data.fragment != nullptr) {
            fragment_ = data.fragment;
        }
    }
    return result;
}

const uri_scheme& uri::scheme(void) const
{
    return scheme_;
}

const char_t* uri::userinfo(void) const
{
    return userinfo_;
}

const char_t* uri::host(void) const
{
    return host_;
}

const uint16_t& uri::port(void) const
{
    return port_;
}

const char_t* uri::path(void) const
{
    return path_;
}

const char_t* uri::query(void) const
{
    return query_;
}

const char_t* uri::fragment(void) const
{
    return fragment_;
}

uri::first_pass_data::first_pass_data(void)
    : scheme(nullptr)
    , scheme_size(0)
    , authority(nullptr)
    , authority_size(0)
    , path(nullptr)
    , path_size(0)
    , query(nullptr)
    , query_size(0)
    , fragment(nullptr)
    , fragment_size(0)
{
}

bool uri::parse_1st_pass(char_t*& raw, size_t& remaining, first_pass_data& data,
                         bool skip_scheme)
{
    if (remaining > 0) {
        static const select_char_map path_query_fragment_map =
            make_select_char_map('/', '?', '#');

        char_t c = *raw;
        if (!path_query_fragment_map[static_cast<uint8_t>(c)]) {
            char_t* scheme_or_authority_data = raw;
            size_t length = parse_uri_word(raw, remaining, ':', '/', '?', '#',
                                           ' ', '\t', '\r', '\n');
            if ((!skip_scheme) && (':' == raw[0])) {
                scheme_or_authority_data[length] = '\0';
                data.scheme = scheme_or_authority_data;
                data.scheme_size = length;
                remaining--;
                raw++;

                skip_optional_slashes(raw, remaining);
            } else {
                length += parse_uri_word(raw, remaining, '/', '?', '#', ' ',
                                         '\t', '\r', '\n');
                c = *raw;
                scheme_or_authority_data[length] = '\0';
                data.authority = scheme_or_authority_data;
                data.authority_size = length;
            }
        }

        if (remaining > 0) {
            if (!path_query_fragment_map[static_cast<uint8_t>(c)]) {
                char_t* authority_data = raw;
                size_t length = parse_uri_word(raw, remaining, '/', '?', '#',
                                               ' ', '\t', '\r', '\n');
                c = *raw;
                authority_data[length] = '\0';
                data.authority = authority_data;
                data.authority_size = length;
            }

            data.path = parse_optional_positional_token(
                raw, remaining, c, data.path_size, '/', '?', '#', ' ', '\t',
                '\r', '\n');
            data.query = parse_optional_positional_token(
                raw, remaining, c, data.query_size, '?', '#', ' ', '\t', '\r',
                '\n');
            data.fragment = parse_optional_positional_token(
                raw, remaining, c, data.fragment_size, '#', ' ', '\t', '\r',
                '\n');
        }
    }

    static const select_char_map empty_map =
        make_select_char_map(' ', '\t', '\r', '\n');
    return (remaining == 0) || (empty_map[static_cast<uint8_t>(*raw)]);
}

bool uri::parse_scheme(const char_t* const scheme_ptr, const size_t& size)
{
    using value_type = std::tuple<uri_scheme, uint16_t>;
    static const trie<value_type> t = make_scheme_trie();

    auto r = t.find(scheme_ptr, size);
    if (r.used_size() == size) {
        std::tie(scheme_, port_) = r.value();
    }

    return uri_scheme::UNKNOWN != scheme_;
}

bool uri::parse_authority(char_t* const authority_ptr, const size_t& size)
{
    // There is an ambiguity in the authority part of a URI specified by
    // RFC3986. You are not able to correctly parse the authority into tokens
    // without looking ahead n symbols, where n is the length of the whole
    // authority part, because the symbol of ':' could occur before and after
    // the '@' symbol, which is also optional. So what do you do, if you have
    // found a ':' symbol without an '@' symbol before? The ':' could be part of
    // the user info or it separates host and port. Therefore it is easier to
    // perform a 2-pass parsing to determine, whether a '@' symbol occurs or
    // not.

    bool result = true;

    char_t* temp_host = authority_ptr;
    size_t host_size = size;
    for (size_t i = 0; i < size; i++) {
        if ('@' == authority_ptr[i]) {
            authority_ptr[i] = '\0';
            userinfo_ = authority_ptr;
            temp_host = authority_ptr + (i + 1);
            host_size = size - (i + 1);
            break;
        }
    }
    host_ = temp_host;

    const char_t* temp_port = nullptr;
    size_t port_size = 0;
    for (size_t i = 0; i < host_size; i++) {
        if (':' == temp_host[i]) {
            temp_host[i] = '\0';
            temp_port = temp_host + (i + 1);
            port_size = host_size - (i + 1);
            break;
        }
    }

    if (port_size > 0) {
        int32_t port_number = parse_unsigned_integer(temp_port, port_size);
        if ((port_size == 0) && (port_number > 0) && (port_number <= 65536)) {
            port_ = static_cast<uint16_t>(port_number);
        } else {
            result = false;
        }
    }

    return result;
}

} // namespace hutzn
