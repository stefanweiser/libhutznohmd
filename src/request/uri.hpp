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

    bool parse(lexer& l, int32_t& character, const bool skip_scheme);

    void set_scheme(const uri_scheme& new_scheme);
    bool set_userinfo(const char_t* const new_userinfo);
    bool set_host(const char_t* const new_host);
    void set_port(const uint16_t& new_port);

    bool valid(void) const;
    const uri_scheme& scheme(void) const;
    const char_t* userinfo(void) const;
    const char_t* host(void) const;
    const uint16_t& port(void) const;
    const char_t* path(void) const;
    const char_t* query(void) const;
    const char_t* fragment(void) const;

private:
    bool parse_scheme_and_authority(int32_t& character, const bool skip_scheme);
    bool parse_scheme(int32_t& ch);
    bool parse_userinfo_and_authority(int32_t& character);
    bool parse_authority(int32_t& character);
    bool parse_authority_1st_pass(int32_t& character);
    bool parse_authority_2nd_pass(void);

    bool parse_word(void);

    lexer* lexer_;
    bool valid_;
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
