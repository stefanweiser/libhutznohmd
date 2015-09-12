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

#ifndef LIBHUTZNOHMD_REQUEST_URI_HPP
#define LIBHUTZNOHMD_REQUEST_URI_HPP

#include <request/lexer.hpp>
#include <http/parser/utility/push_back_string.hpp>

#include <hutzn.hpp>

namespace hutzn
{

//! Implements parsing of URIs as specified in RFC 3986. It supports only the
//! specified schemes.
class uri
{
public:
    explicit uri(void);

    explicit uri(const uri& rhs) = delete;
    uri& operator=(const uri& rhs) = delete;

    bool parse(lexer& lex, int32_t& character, const bool skip_scheme);

    const uri_scheme& scheme(void) const;
    const char_t* userinfo(void) const;
    const char_t* host(void) const;
    const uint16_t& port(void) const;
    const char_t* path(void) const;
    const char_t* query(void) const;
    const char_t* fragment(void) const;

private:
    size_t prepare_uri_data(char_t* const data, const size_t length);

    bool parse_scheme_and_authority(lexer& lex, int32_t& character,
                                    const bool skip_scheme);
    bool parse_scheme(lexer& lex, int32_t& ch);
    bool parse_userinfo_and_authority(lexer& lex, int32_t& character);
    bool parse_authority(lexer& lex, int32_t& character);
    bool parse_authority_1st_pass(lexer& lex, int32_t& character);
    bool parse_authority_2nd_pass(void);

    static bool is_path_separator(const int32_t ch);
    static bool is_query_separator(const int32_t ch);
    static bool is_fragment_separator(const int32_t ch);

    bool already_called_;
    uri_scheme scheme_;
    http::push_back_string<16> userinfo_;
    http::push_back_string<32> host_;
    uint16_t port_;
    http::push_back_string<32> path_;
    http::push_back_string<32> query_;
    http::push_back_string<32> fragment_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_URI_HPP
