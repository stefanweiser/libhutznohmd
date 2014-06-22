/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <http/parser/utility/trie.hpp>

#include "uri.hpp"

namespace rest
{

namespace http
{

namespace
{

//! Parses a URI specific word stopping if the continue condition gets wrong.
template<size_t size, typename continue_function>
bool parse_uri_word(int32_t & character,
                    push_back_string<size> & result,
                    const continue_function & continue_condition_functor,
                    const lexer & l)
{
    while ((character >= 0) &&
           (true == continue_condition_functor(static_cast<char>(character)))) {
        if ('%' == character) {
            int32_t a = l.get();
            int32_t b = l.get();
            if ((a == -1) || (b == -1)) {
                return false;
            }

            int32_t c = (from_hex(static_cast<char>(a)) << 4) + from_hex(static_cast<char>(b));
            result.push_back(static_cast<char>(c));
        } else {
            result.push_back(static_cast<char>(character));
        }
        character = l.get();
    }

    return true;
}

} // namespace

uri::uri(const lexer & l)
    : lexer_(l)
    , scheme_(uri_scheme::UNKNOWN)
    , userinfo_()
    , host_()
    , port_(0)
    , path_()
    , query_()
    , fragment_()
{}

bool uri::parse(int32_t & character)
{
    if ('/' != character) {
        if (false == parse_scheme(character)) {
            return false;
        }

        if (':' != character) {
            return false;
        }
    }

    character = lexer_.get();
    if (character < 0) {
        return false;
    }

    if ('/' == character) {
        const int32_t last_character = character;
        path_.push_back(static_cast<char>(character));
        character = lexer_.get();

        if (('/' == last_character) && ('/' == character)) {
            // It is no path. It is an authority.
            path_.clear();
            character = lexer_.get();
            if (false == parse_authority(character)) {
                return false;
            }
        }
    }

    if (('?' != character) && ('#' != character)) {
        // Must be a path or the end of the URI.
        if (false == parse_uri_word(character, path_, &is_valid_uri_path_character, lexer_)) {
            return false;
        }
    }

    if ('?' == character) {
        character = lexer_.get();
        if (false == parse_uri_word(character, query_, &is_valid_uri_query_character, lexer_)) {
            return false;
        }
    }

    if ('#' == character) {
        character = lexer_.get();
        if (false == parse_uri_word(character,
                                    fragment_,
                                    &is_valid_uri_fragment_character,
                                    lexer_)) {
            return false;
        }
    }

    return true;
}

bool uri::parse_scheme(int32_t & character)
{
    using value_info = trie<rest::http::uri_scheme>::value_info;
    static const std::vector<value_info> types = {{
            value_info{"http", rest::http::uri_scheme::HTTP},
            value_info{"mailto", rest::http::uri_scheme::MAILTO}
        }
    };

    static const trie<rest::http::uri_scheme> t(types, rest::http::uri_scheme::UNKNOWN);
    push_back_string<8> tmp;
    scheme_ = t.parse(character, tmp, lexer_);
    return (uri_scheme::UNKNOWN != scheme_);
}

bool uri::parse_authority(int32_t & character)
{
    if (false == parse_uri_word(character, userinfo_, &is_valid_uri_userinfo_character, lexer_)) {
        return false;
    }

    if ('@' != character) {
        // This authority contains no userinfo. All we have parsed till here is part of the host.
        host_.push_back(userinfo_.c_str());
        userinfo_.clear();
    } else {
        character = lexer_.get();
    }

    if (false == parse_uri_word(character, host_, &is_valid_uri_host_character, lexer_)) {
        return false;
    }

    if (':' == character) {
        // A port is appended.
        character = lexer_.get();
        const int32_t p = lexer_.get_unsigned_integer(character);
        if ((p > 0) && (p < 65536)) {
            port_ = static_cast<uint16_t>(p);
        }
    }

    return true;
}

} // namespace http

} // namespace rest
