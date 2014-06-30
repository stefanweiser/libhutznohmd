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

            char d = from_hex(static_cast<char>(a));
            char e = from_hex(static_cast<char>(b));
            if ((d == -1) || (e == -1)) {
                return false;
            }

            result.push_back(static_cast<char>((d << 4) + e));
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
    if (false == parse_scheme_and_authority(character)) {
        return false;
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

void uri::set_scheme(const uri_scheme & new_scheme)
{
    scheme_ = new_scheme;
}

const uri_scheme & uri::scheme() const
{
    return scheme_;
}

const push_back_string<16> & uri::userinfo() const
{
    return userinfo_;
}

const push_back_string<32> & uri::host() const
{
    return host_;
}

const uint16_t & uri::port() const
{
    return port_;
}

const push_back_string<32> & uri::path() const
{
    return path_;
}

const push_back_string<32> & uri::query() const
{
    return query_;
}

const push_back_string<32> & uri::fragment() const
{
    return fragment_;
}


bool uri::parse_scheme_and_authority(int32_t & character)
{
    // Check whether there is a scheme and authority or neither of them. This is not conform with
    // RFC 3986, but HTTP specifies request URIs without scheme and authority.
    if ('/' != character) {
        if (false == parse_scheme(character)) {
            return false;
        }

        if (':' != character) {
            return false;
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
    // There is an ambiguity in the authority part of an URI specified by RFC3986. You are not
    // able to correctly parse the the authority into tokens without looking ahead n symbols,
    // where n is the length of the whole authority part, because the symbol of ':' could occur
    // before and after the '@' symbol, which is also optional. So what do you do, if you have
    // found a ':' symbol without an '@' symbol before? The ':' could be part of the user info or
    // it separates host and port.
    // Therefore it is easier to perform a 2-pass parsing to determine, whether a '@' symbol
    // occurs or not.

    if (false == parse_authority_1st_pass(character)) {
        return false;
    }

    return parse_authority_2nd_pass();
}

bool uri::parse_authority_1st_pass(int32_t & character)
{
    if (false == parse_uri_word(character,
                                host_,
                                &is_valid_uri_authority_character,
                                lexer_)) {
        return false;
    }

    if ('@' == character) {
        userinfo_.push_back(host_.c_str());
        host_.clear();
        character = lexer_.get();
    }

    if (false == parse_uri_word(character,
                                host_,
                                &is_valid_uri_authority_character,
                                lexer_)) {
        return false;
    }

    return true;
}

bool uri::parse_authority_2nd_pass()
{
    // Now there are all parts of the authority at the right place, except the port number, if it
    // exists. We will search the host backwards for a number and search the ':' symbol.
    uint32_t p = 0;
    uint32_t factor = 1;
    for (ssize_t i = (host_.size() - 1); i >= 0; i--) {
        const uint8_t c = static_cast<uint8_t>(host_[i] - '0');

        if ((c < 10) && (factor <= 10000)) {

            p += (factor * c);
            factor *= 10;

        } else if ((':' == host_[i]) && (p > 0) && (p < 65536)) {

            port_ = static_cast<uint16_t>(p);

            push_back_string<32> tmp;
            host_[i] = '\0';
            tmp.push_back(host_.c_str());
            host_.clear();
            host_.push_back(tmp.c_str());

            break;

        } else if (':' == host_[i]) {
            // This host is erroneous, because it has no correct port component, but a ':' symbol.
            return false;
        }
    }

    return true;
}

} // namespace http

} // namespace rest
