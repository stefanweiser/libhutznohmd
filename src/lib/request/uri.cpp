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

#include "uri.hpp"

#include <tuple>

#include "utility/character_validation.hpp"
#include "utility/parsing.hpp"
#include "utility/select_char_map.hpp"
#include "utility/trie.hpp"

namespace hutzn
{

namespace
{

//! Makes and returns a newly created scheme trie.
static trie<std::tuple<uri_scheme, uint16_t>> make_scheme_trie()
{
    using value_type = std::tuple<uri_scheme, uint16_t>;
    static const value_type http_value{uri_scheme::HTTP, 80};
    static const value_type https_value{uri_scheme::HTTPS, 443};

    trie<value_type> t{true};
    t.insert("http", http_value);
    t.insert("https", https_value);
    return t;
}

inline bool convert_char(const char_t*& source, char_t*& destination,
                         size_t& head, size_t& tail)
{
    static const uint8_t hex_digit_count = 16;

    bool is_error;
    head++;
    uint8_t a = static_cast<uint8_t>(from_hex(source[head]));
    head++;
    uint8_t b = static_cast<uint8_t>(from_hex(source[head]));
    if ((a < hex_digit_count) && (b < hex_digit_count)) {
        destination[tail] =
            static_cast<char_t>(static_cast<uint8_t>((a << nibble_size) + b));
        head++;
        tail++;
        is_error = false;
    } else {
        tail = 0;
        is_error = true;
    }
    return is_error;
}

inline void skip_optional_slashes(const char_t*& raw, size_t& remaining)
{
    static const uint8_t optional_slash_size = 2;
    if ((remaining >= optional_slash_size) && ('/' == raw[0]) &&
        ('/' == raw[1])) {
        remaining -= optional_slash_size;
        raw += optional_slash_size;
    }
}

//! Parses maximal remaining characters of data and stops, when one of the
//! selected characters is discovered.
template <typename... tn>
size_t parse_uri_word(const char_t*& source, size_t& source_remaining,
                      char_t*& destination, size_t& destination_remaining,
                      const tn... select_chars)
{
    static const select_char_map map = make_select_char_map(select_chars...);

    size_t head = 0;
    size_t tail = 0;
    bool stop = false;
    while (!stop) {
        const char_t ch = source[head];
        if ((map[static_cast<uint8_t>(ch)]) || (head >= source_remaining) ||
            (tail >= destination_remaining)) {
            stop = true;
        } else if ('%' == ch) {
            static const uint8_t char_encoding_size = 3;
            if ((source_remaining - head) >= char_encoding_size) {
                stop = convert_char(source, destination, head, tail);
            } else {
                tail = 0;
                stop = true;
            }
        } else {
            destination[tail] = source[head];
            head++;
            tail++;
        }
    }

    destination[tail] = '\0';
    tail++;

    source_remaining -= head;
    destination_remaining -= tail;
    source += head;
    destination += tail;
    return tail - 1;
}

template <typename... tn>
char_t* parse_optional_positional_token(
    const char_t*& source, size_t& source_remaining, char_t*& destination,
    size_t& destination_remaining, char_t& last_char, size_t& result_size,
    const char_t conditional_start_character, const tn... select_chars)
{
    char_t* result = NULL;
    if ((source_remaining > 0) && (destination_remaining > 0) &&
        (last_char == conditional_start_character)) {
        source_remaining--;
        source++;

        char_t* original_destination = destination;
        size_t length = parse_uri_word(source, source_remaining, destination,
                                       destination_remaining, select_chars...);
        last_char = *source;
        result = original_destination;
        result_size = length;
    }
    return result;
}

} // namespace

uri::uri(void)
    : already_called_(false)
    , scheme_(uri_scheme::UNKNOWN)
    , userinfo_(NULL)
    , host_(NULL)
    , port_(0)
    , path_(NULL)
    , query_(NULL)
    , fragment_(NULL)
{
}

//! @todo seperate http and mailto uri parser to get rid of skip_scheme param.
bool uri::parse(const char_t* source, size_t source_length, char_t* destination,
                size_t destination_length, bool skip_scheme)
{
    bool result = false;
    if ((source_length + 2) <= destination_length) {
        first_pass_data data;
        const bool passed_first_pass =
            parse_1st_pass(source, source_length, destination,
                           destination_length, data, skip_scheme);
        result = passed_first_pass;
        if (passed_first_pass) {
            // This implementation supports both: URIs with and without scheme
            // or authority. Though it is not strictly conforming with RFC 3986,
            // HTTP specifies request URIs without scheme and authority.
            if ((data.scheme_size > 0) &&
                (!parse_scheme(data.scheme, data.scheme_size))) {
                result = false;
            }
            if ((data.authority_size > 0) &&
                (!parse_authority(data.authority, data.authority_size))) {
                result = false;
            }
            if (data.path != NULL) {
                path_ = data.path;
            }
            if (data.query != NULL) {
                query_ = data.query;
            }
            if (data.fragment != NULL) {
                fragment_ = data.fragment;
            }
        }
    }
    return result;
}

uri_scheme uri::scheme(void) const
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

uint16_t uri::port(void) const
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

bool uri::parse_1st_pass(const char_t* source, size_t source_length,
                         char_t*& destination, size_t& destination_length,
                         first_pass_data& data, bool skip_scheme)
{
    if ((source_length > 0) && (destination_length > 0)) {
        static const select_char_map path_query_fragment_map =
            make_select_char_map('/', '?', '#');

        char_t c = *source;
        if (!path_query_fragment_map[static_cast<uint8_t>(c)]) {
            char_t* scheme_or_authority_data = destination;
            size_t length = parse_uri_word(source, source_length, destination,
                                           destination_length, ':', '/', '?',
                                           '#', ' ', '\t', '\r', '\n', '\0');
            if ((!skip_scheme) && (':' == source[0])) {
                data.scheme = scheme_or_authority_data;
                data.scheme_size = length;
                source_length--;
                source++;

                skip_optional_slashes(source, source_length);
            } else {
                destination--;
                destination_length++;
                length += parse_uri_word(source, source_length, destination,
                                         destination_length, '/', '?', '#', ' ',
                                         '\t', '\r', '\n', '\0');
                c = *source;
                data.authority = scheme_or_authority_data;
                data.authority_size = length;
            }
        }

        if ((source_length > 0) && (destination_length > 0)) {
            if (!path_query_fragment_map[static_cast<uint8_t>(c)]) {
                char_t* authority_data = destination;
                const size_t length = parse_uri_word(
                    source, source_length, destination, destination_length, '/',
                    '?', '#', ' ', '\t', '\r', '\n', '\0');
                c = *source;
                data.authority = authority_data;
                data.authority_size = length;
            }

            if ((source_length > 0) && (destination_length > 0) && (c == '/')) {
                char_t* original_destination = destination;
                size_t length = parse_uri_word(
                    source, source_length, destination, destination_length, '?',
                    '#', ' ', '\t', '\r', '\n', '\0');
                c = *source;
                data.path = original_destination;
                data.path_size = length;
            }

            data.query = parse_optional_positional_token(
                source, source_length, destination, destination_length, c,
                data.query_size, '?', '#', ' ', '\t', '\r', '\n', '\0');
            data.fragment = parse_optional_positional_token(
                source, source_length, destination, destination_length, c,
                data.fragment_size, '#', ' ', '\t', '\r', '\n', '\0');
        }
    }

    static const select_char_map empty_map =
        make_select_char_map(' ', '\t', '\r', '\n', '\0');
    return (source_length == 0) || empty_map[static_cast<uint8_t>(*source)];
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

    const char_t* temp_port = NULL;
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
        int32_t port_number =
            parse_unsigned_integer<int32_t>(temp_port, port_size);
        if ((port_size == 0) &&
            (port_number > std::numeric_limits<uint16_t>::min()) &&
            (port_number <= std::numeric_limits<uint16_t>::max())) {
            port_ = static_cast<uint16_t>(port_number);
        } else {
            result = false;
        }
    }

    return result;
}

} // namespace hutzn
