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
#include <utility/trie.hpp>

#include "uri.hpp"

namespace hutzn
{

namespace
{

//! Parses a URI specific word stopping if the continue condition gets wrong.
template <size_t size, typename continue_function>
bool parse_uri_word(int32_t& character, http::push_back_string<size>& result,
                    const continue_function& continue_condition_functor,
                    lexer& l)
{
    while ((character >= 0) && (true == continue_condition_functor(
                                            static_cast<uint8_t>(character)))) {
        if ('%' == character) {
            int32_t a = l.get();
            int32_t b = l.get();
            if ((a == -1) || (b == -1)) {
                return false;
            }

            char_t d = from_hex(static_cast<char_t>(a));
            char_t e = from_hex(static_cast<char_t>(b));
            if ((d == -1) || (e == -1)) {
                return false;
            }

            result.push_back(static_cast<char_t>((d << 4) + e));
        } else {
            result.push_back(static_cast<char_t>(character));
        }
        character = l.get();
    }

    return true;
}

} // namespace

uri::uri(void)
    : valid_(false)
    , scheme_(uri_scheme::UNKNOWN)
    , userinfo_()
    , host_()
    , port_(0)
    , path_()
    , query_()
    , fragment_()
{
}

bool uri::parse(lexer& lex, int32_t& ch, const bool skip_scheme)
{
    if (true == valid_) {
        return true;
    }

    if (false == parse_scheme_and_authority(lex, ch, skip_scheme)) {
        return false;
    }

    if ((false == is_query_seperator(ch)) &&
        (false == is_fragment_seperator(ch))) {
        // Must be a path or the end of the URI.
        if (false == parse_uri_word(ch, path_,
                                    &is_valid_uri_path_character, lex)) {
            return false;
        }
    }

    if (false == is_query_seperator(ch)) {
        ch = lex.get();
        if (false == parse_uri_word(ch, query_,
                                    &is_valid_uri_query_character, lex)) {
            return false;
        }
    }

    if (false == is_fragment_seperator(ch)) {
        ch = lex.get();
        if (false == parse_uri_word(ch, fragment_,
                                    &is_valid_uri_fragment_character, lex)) {
            return false;
        }
    }

    valid_ = true;
    return true;
}

void uri::set_scheme(const uri_scheme& new_scheme)
{
    scheme_ = new_scheme;
}

bool uri::set_userinfo(const char_t* const new_userinfo)
{
    bool new_userinfo_is_valid = true;
    const char_t* c = new_userinfo;
    while ('\0' != (*c)) {
        if (false ==
            is_valid_uri_authority_character(static_cast<uint8_t>(*c))) {
            new_userinfo_is_valid = false;
        }
        c++;
    }

    if (true == new_userinfo_is_valid) {
        userinfo_.clear();
        userinfo_.append_string(new_userinfo);
    }

    return new_userinfo_is_valid;
}

bool uri::set_host(const char_t* const new_host)
{
    bool new_host_is_valid = true;
    const char_t* c = new_host;
    while ('\0' != (*c)) {
        if (false ==
            is_valid_uri_authority_character(static_cast<uint8_t>(*c))) {
            new_host_is_valid = false;
        }
        c++;
    }

    if (true == new_host_is_valid) {
        host_.clear();
        host_.append_string(new_host);
    }

    return new_host_is_valid;
}

void uri::set_port(const uint16_t& new_port)
{
    port_ = new_port;
}

bool uri::valid(void) const
{
    return valid_;
}

const uri_scheme& uri::scheme(void) const
{
    return scheme_;
}

const char_t* uri::userinfo(void) const
{
    return userinfo_.c_str();
}

const char_t* uri::host(void) const
{
    return host_.c_str();
}

const uint16_t& uri::port(void) const
{
    return port_;
}

const char_t* uri::path(void) const
{
    return path_.c_str();
}

const char_t* uri::query(void) const
{
    return query_.c_str();
}

const char_t* uri::fragment(void) const
{
    return fragment_.c_str();
}

bool uri::parse_scheme_and_authority(lexer& lex, int32_t& character,
                                     const bool skip_scheme)
{
    // Check whether there is a scheme and authority or neither of them. This is
    // not conform with RFC 3986, but HTTP specifies request URIs without scheme
    // and authority.
    if ('/' != character) {
        if (false == skip_scheme) {
            if (false == parse_scheme(lex, character)) {
                return false;
            }

            character = lex.get();
            if (character < 0) {
                return false;
            }

            if (false == parse_userinfo_and_authority(lex, character)) {
                return false;
            }
        } else {
            if (false == parse_authority(lex, character)) {
                return false;
            }
        }
    }

    return true;
}

bool uri::parse_scheme(lexer& lex, int32_t& ch)
{
    using value_type = std::tuple<uri_scheme, uint16_t>;
    trie<value_type> t{true};
    t.insert("http", value_type(uri_scheme::HTTP, 80));
    t.insert("https", value_type(uri_scheme::HTTPS, 443));
    t.insert("mailto", value_type(uri_scheme::MAILTO, 0));

    auto equals_scheme_seperator =
        [](const char_t c) -> bool { return (':' == c); };

    const size_t begin_index = lex.prev_index();
    const size_t length = parse_specific(lex, ch, equals_scheme_seperator);

    auto r = t.find(lex.header_data(begin_index), length);
    if (r.used_size() == length) {
        std::tie(scheme_, port_) = r.value();
    }

    return (uri_scheme::UNKNOWN != scheme_);
}

bool uri::parse_userinfo_and_authority(lexer& lex, int32_t& character)
{
    bool result = true;
    if ('/' == character) {
        path_.push_back(static_cast<char_t>(character));
        character = lex.get();
        if ('/' == character) {
            // It is no path. It is an authority.
            path_.clear();
            character = lex.get();
            if (false == parse_authority(lex, character)) {
                result = false;
            }
        }
    }
    return result;
}

bool uri::parse_authority(lexer& lex, int32_t& character)
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

    bool result = parse_authority_1st_pass(lex, character);
    if (true == result) {
        result = parse_authority_2nd_pass();
    }
    return result;
}

bool uri::parse_authority_1st_pass(lexer& lex, int32_t& character)
{
    if (false == parse_uri_word(character, host_,
                                &is_valid_uri_authority_character, lex)) {
        return false;
    }

    if ('@' == character) {
        userinfo_.append_string(host_.c_str());
        host_.clear();
        character = lex.get();
    }

    if (false == parse_uri_word(character, host_,
                                &is_valid_uri_authority_character, lex)) {
        return false;
    }

    return true;
}

bool uri::parse_authority_2nd_pass(void)
{
    // Now there are all parts of the authority at the right place, except the
    // port number, if it exists. We will search the host backwards for a
    // number and search the ':' symbol.
    uint32_t p = 0;
    uint32_t factor = 1;
    for (ssize_t i = (static_cast<ssize_t>(host_.size()) - 1); i >= 0; i--) {
        const uint8_t c =
            static_cast<uint8_t>(host_[static_cast<size_t>(i)] - '0');

        if ((c < 10) && (factor <= 10000)) {

            p += (factor * c);
            factor *= 10;

        } else if ((':' == host_[static_cast<size_t>(i)]) && (p > 0) &&
                   (p < 65536)) {

            port_ = static_cast<uint16_t>(p);

            http::push_back_string<32> tmp;
            host_[static_cast<size_t>(i)] = '\0';
            tmp.append_string(host_.c_str());
            host_.clear();
            host_.append_string(tmp.c_str());

            break;

        } else if (':' == host_[static_cast<size_t>(i)]) {
            // This host is erroneous, because it has no correct port component,
            // but a ':' symbol.
            return false;
        }
    }

    return true;
}

bool uri::is_query_seperator(const int32_t ch)
{
    return (static_cast<int32_t>('?') == ch);
}

bool uri::is_fragment_seperator(const int32_t ch)
{
    return (static_cast<int32_t>('#') == ch);
}

} // namespace hutzn
