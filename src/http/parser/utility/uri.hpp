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

#ifndef LIBHUTZNOHMD_HTTP_PARSER_UTILITY_URI_HPP
#define LIBHUTZNOHMD_HTTP_PARSER_UTILITY_URI_HPP

#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/push_back_string.hpp>

#include <hutzn.hpp>

namespace hutzn
{

namespace http
{

//! Implements parsing of URIs as specified in RFC 3986. It supports only the
//! specified schemes.
class uri : public uri_interface
{
public:
    explicit uri();

    explicit uri(const uri& rhs) = delete;
    uri& operator=(const uri& rhs) = delete;

    bool parse(const lexer& l, int32_t& character, const bool skip_scheme);

    void set_scheme(const uri_scheme& new_scheme);
    bool set_userinfo(const char* const new_userinfo);
    bool set_host(const char* const new_host);
    void set_port(const uint16_t& new_port);

    bool valid() const override;
    const uri_scheme& scheme() const override;
    const char* userinfo() const override;
    const char* host() const override;
    const uint16_t& port() const override;
    const char* path() const override;
    const char* query() const override;
    const char* fragment() const override;

private:
    bool parse_scheme_and_authority(int32_t& character, const bool skip_scheme);
    bool parse_scheme(int32_t& character);
    bool parse_userinfo_and_authority(int32_t& character);
    bool parse_authority(int32_t& character);
    bool parse_authority_1st_pass(int32_t& character);
    bool parse_authority_2nd_pass();

    bool parse_word();

    const lexer* lexer_;
    bool valid_;
    uri_scheme scheme_;
    push_back_string<16> userinfo_;
    push_back_string<32> host_;
    uint16_t port_;
    push_back_string<32> path_;
    push_back_string<32> query_;
    push_back_string<32> fragment_;
};

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HTTP_PARSER_UTILITY_URI_HPP
