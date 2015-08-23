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

#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/push_back_string.hpp>

#include <hutzn.hpp>

namespace hutzn
{

//! Implements parsing of URIs as specified in RFC 3986. It supports only the
//! specified schemes.
class uri
{
public:
    explicit uri();

    explicit uri(const uri& rhs) = delete;
    uri& operator=(const uri& rhs) = delete;

    bool parse(const http::lexer& l, int32_t& character,
               const bool skip_scheme);

    void set_scheme(const http::uri_scheme& new_scheme);
    bool set_userinfo(const char_t* const new_userinfo);
    bool set_host(const char_t* const new_host);
    void set_port(const uint16_t& new_port);

    bool valid() const;
    const http::uri_scheme& scheme() const;
    const char_t* userinfo() const;
    const char_t* host() const;
    const uint16_t& port() const;
    const char_t* path() const;
    const char_t* query() const;
    const char_t* fragment() const;

private:
    bool parse_scheme_and_authority(int32_t& character, const bool skip_scheme);
    bool parse_scheme(int32_t& character);
    bool parse_userinfo_and_authority(int32_t& character);
    bool parse_authority(int32_t& character);
    bool parse_authority_1st_pass(int32_t& character);
    bool parse_authority_2nd_pass();

    bool parse_word();

    const http::lexer* lexer_;
    bool valid_;
    http::uri_scheme scheme_;
    http::push_back_string<16> userinfo_;
    http::push_back_string<32> host_;
    uint16_t port_;
    http::push_back_string<32> path_;
    http::push_back_string<32> query_;
    http::push_back_string<32> fragment_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_URI_HPP
