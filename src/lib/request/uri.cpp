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

//! @brief Makes and returns a newly created scheme trie.
//!
//! @return Trie of uri scheme, port tuples.
trie<std::tuple<uri_scheme, uint16_t>> make_scheme_trie()
{
    using value_type = std::tuple<uri_scheme, uint16_t>;
    static const value_type http_value{uri_scheme::HTTP, 80};
    static const value_type https_value{uri_scheme::HTTPS, 443};

    trie<value_type> t{true};
    t.insert("http", http_value);
    t.insert("https", https_value);
    return t;
}

//! @brief Consumes one character.
//!
//! This will set the data pointer one character forth and decreases the
//! remaining size.
//! @warning There is no over- or underflow checking.
//! @param[in,out] data      Points to a buffer.
//! @param[in,out] remaining Remaining size of the buffer.
void consume_one_character(const char_t*& data, size_t& remaining)
{
    data++;
    remaining--;
}

//! @brief Reverts one character consumation.
//!
//! This will set the data pointer one character backwards and increases the
//! remaining size.
//! @warning There is no over- or underflow checking.
//! @param[in,out] data      Points to a buffer.
//! @param[in,out] remaining Remaining size of the buffer.
void unconsume_one_character(char_t*& data, size_t& remaining)
{
    data--;
    remaining++;
}

//! @brief Converts a percent encoded character into a byte.
//!
//! @warning Expects, that at least 2 more bytes after the current one will
//! follow and that the current character is a "percent".
//! @param[in] source                Source pointer where to read the characters
//!                                  from.
//! @param[in,out] destination       Points to the destination buffer of the
//!                                  converted character.
//! @param[in,out] source_index      Current index in the source buffer.
//! @param[in,out] destination_index Current index in the destination buffer.
//! @return True if the conversion was successful and false when not.
inline bool convert_char(const char_t* const source, char_t*& destination,
                         size_t& source_index, size_t& destination_index)
{
    static const uint8_t hex_digit_count = 16;

    bool is_error;

    // skip the percent character
    source_index++;

    // get the first hex-digit
    uint8_t a = static_cast<uint8_t>(from_hex(source[source_index]));
    source_index++;

    // get the second hex-digit
    uint8_t b = static_cast<uint8_t>(from_hex(source[source_index]));
    source_index++;

    // check the hex digits for validity
    if ((a < hex_digit_count) && (b < hex_digit_count)) {
        // write it to destination when valid and increase the index once
        destination[destination_index] =
            static_cast<char_t>(static_cast<uint8_t>((a << nibble_size) + b));
        destination_index++;
        is_error = false;
    } else {
        // digits are not valid
        is_error = true;
    }
    return is_error;
}

//! @brief Skips slashes after the sheme part if they are present.
//!
//! @param[in,out] raw       Source buffer.
//! @param[in,out] remaining Remaining bytes in the source buffer.
inline void skip_optional_slashes(const char_t*& raw, size_t& remaining)
{
    // skips character when there are at least two characters to come and both
    // actually are slashes
    static const uint8_t optional_slash_size = 2;
    if ((remaining >= optional_slash_size) && ('/' == raw[0]) &&
        ('/' == raw[1])) {
        remaining -= optional_slash_size;
        raw += optional_slash_size;
    }
}

//! @brief Parses an uri word.
//!
//! This means all remaining characters get parsed until one of the selected
//! characters or the end of the string is discovered. The word gets
//! nullterminated in the destination buffer afterwards.
//! @param[in,out] source                Points to the source buffer.
//! @param[in,out] source_remaining      Number of bytes remaining to parse.
//! @param[in,out] destination           Points to the destination buffer.
//! @param[in,out] destination_remaining Number of bytes available to write.
//! @param[in] select_chars              Several character, that will stop
//!                                      parsing.
//! @return                              The length of the written word.
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

        // if the read character is a stop character it's a reason to stop
        if ((map[static_cast<uint8_t>(ch)]) || (head >= source_remaining) ||
            (tail >= destination_remaining)) {
            stop = true;
        } else if ('%' == ch) {
            // in case of a percent-encoding
            static const uint8_t char_encoding_size = 3;
            if ((source_remaining - head) >= char_encoding_size) {
                stop = convert_char(source, destination, head, tail);
            } else {
                tail = 0;
                stop = true;
            }
        } else {
            // in any other case simply copy the character
            destination[tail] = source[head];
            head++;
            tail++;
        }
    }

    // the string gets zero-terminated
    destination[tail] = '\0';
    tail++;

    // update pointer and counters
    source_remaining -= head;
    destination_remaining -= tail;
    source += head;
    destination += tail;

    // the zero-termination will not count for the length of the uri word
    return tail - 1;
}

//! @brief Parses an optional positional token.
//!
//! This is either path, query string or fragment. These tokens get parsed, when
//! a start character is present.
//! @param[in,out] source                  Points to the source buffer.
//! @param[in,out] source_remaining        Number of bytes remaining to parse.
//! @param[in,out] destination             Points to the destination buffer.
//! @param[in,out] destination_remaining   Number of bytes available to write.
//! @param[in,out] result_size             Length of the result if not NULL.
//! @param[in] conditional_start_character Source must start with this character
//!                                        or the token is not present.
//! @param[in] skip_one_character          The first character does not get part
//!                                        of the resultant string in the
//!                                        destination buffer.
//! @param[in] select_chars                Several character, that will stop
//!                                        parsing.
//! @return                                A pointer to the resultant token.
template <typename... tn>
char_t* parse_optional_positional_token(
    const char_t*& source, size_t& source_remaining, char_t*& destination,
    size_t& destination_remaining, size_t& result_size,
    const char_t conditional_start_character, const bool skip_one_character,
    const tn... select_chars)
{
    // the token is present when the conditional_start_character is found at the
    // begin
    char_t* result = NULL;
    if ((source_remaining > 0) && (destination_remaining > 0) &&
        ((*source) == conditional_start_character)) {
        if (skip_one_character) {
            consume_one_character(source, source_remaining);
        }

        char_t* original_destination = destination;
        size_t length = parse_uri_word(source, source_remaining, destination,
                                       destination_remaining, select_chars...);
        result = original_destination;
        result_size = length;
    }
    return result;
}

} // namespace

uri::uri(void)
    : scheme_(uri_scheme::UNKNOWN)
    , userinfo_(NULL)
    , host_(NULL)
    , port_(0)
    , path_(NULL)
    , query_(NULL)
    , fragment_(NULL)
{
}

//! @todo implement RFC 3986 correctly.
bool uri::parse(const char_t* source, size_t source_length, char_t* destination,
                size_t destination_length, const bool skip_scheme)
{
    bool result = false;
    static const size_t maximum_uri_enlargement = 3;
    if (((source_length + maximum_uri_enlargement) <= destination_length) &&
        (source_length > 0) && (destination_length > 0)) {
        first_pass_data data;
        const bool passed_first_pass =
            parse_1st_pass(source, source_length, destination,
                           destination_length, data, skip_scheme);
        result = passed_first_pass;
        if (passed_first_pass) {
            // this implementation supports both: uris with and without scheme
            // or authority
            // though it is not strictly conforming with RFC 3986 HTTP also
            // specifies request uris without scheme and authority
            if ((data.scheme_size > 0) &&
                (!parse_scheme(data.scheme, data.scheme_size))) {
                result = false;
            }
            if ((data.authority_size > 0) &&
                (!parse_authority(data.authority, data.authority_size))) {
                result = false;
            }
            path_ = data.path;
            query_ = data.query;
            fragment_ = data.fragment;
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
                         char_t* destination, size_t destination_length,
                         first_pass_data& data, const bool skip_scheme)
{
    static const select_char_map path_query_fragment_map =
        make_select_char_map('/', '?', '#');

    // first check whether it begins already the path, query string or fragment
    if (!path_query_fragment_map[static_cast<uint8_t>(*source)]) {
        char_t* scheme_or_authority_data = destination;
        size_t length = parse_uri_word(source, source_length, destination,
                                       destination_length, ':', '/', '?', '#',
                                       ' ', '\t', '\r', '\n', '\0');

        // check if this is the scheme or the authority part
        char_t* authority_data;
        if ((!skip_scheme) && (':' == (*source))) {
            data.scheme = scheme_or_authority_data;
            data.scheme_size = length;
            length = 0;
            consume_one_character(source, source_length);

            skip_optional_slashes(source, source_length);
            authority_data = destination;
        } else {
            unconsume_one_character(destination, destination_length);
            authority_data = scheme_or_authority_data;
        }

        // if the authority part was not already parsed
        if (!path_query_fragment_map[static_cast<uint8_t>(*source)]) {
            length += parse_uri_word(source, source_length, destination,
                                     destination_length, '/', '?', '#', ' ',
                                     '\t', '\r', '\n', '\0');

            if (length > 0) {
                data.authority = authority_data;
                data.authority_size = length;
            }
        }
    }

    // parse path
    data.path = parse_optional_positional_token(
        source, source_length, destination, destination_length, data.path_size,
        '/', false, '?', '#', ' ', '\t', '\r', '\n', '\0');

    // parse query string
    data.query = parse_optional_positional_token(
        source, source_length, destination, destination_length, data.query_size,
        '?', true, '#', ' ', '\t', '\r', '\n', '\0');

    // parse fragment
    data.fragment = parse_optional_positional_token(
        source, source_length, destination, destination_length,
        data.fragment_size, '#', true, ' ', '\t', '\r', '\n', '\0');

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
    // there is an ambiguity in the authority part of an uri specified by
    // RFC 3986: you are not able to correctly split the authority into tokens
    // without looking ahead n symbols where n is the length of the whole
    // authority part
    //
    // this is because of the symbol ':' that could occur before and after
    // the '@' symbol which is also optional
    //
    // so what's to do if you have found a ':' symbol without an '@' symbol
    // before: the ':' could be part of the user info or it separates host and
    // port
    //
    // therefore it is easier to perform a 2-pass parsing to determine whether
    // an '@' symbol occurs or not.

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
