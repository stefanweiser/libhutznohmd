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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_URI_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_URI_HPP__

#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/pushbackstring.hpp>

namespace rest
{

namespace http
{

//! These URI schemes are supported by the parser and needed for HTTP.
enum class uri_scheme
{
    UNKNOWN = 0,
    HTTP = 1,
    MAILTO = 2
};

//! Implements parsing of URIs as specified in RFC 3986. It supports only the specified schemes.
class uri
{
public:
    explicit uri(const lexer & l);

    bool parse(int32_t & character);

    void set_scheme(const uri_scheme & new_scheme);

    const uri_scheme & scheme() const;
    const push_back_string<16> & userinfo() const;
    const push_back_string<32> & host() const;
    const uint16_t & port() const;
    const push_back_string<32> & path() const;
    const push_back_string<32> & query() const;
    const push_back_string<32> & fragment() const;

private:
    bool parse_scheme_and_authority(int32_t & character);
    bool parse_scheme(int32_t & character);
    bool parse_authority(int32_t & character);
    bool parse_authority_1st_pass(int32_t & character);
    bool parse_authority_2nd_pass();

    bool parse_word();

    const lexer & lexer_;
    uri_scheme scheme_;
    push_back_string<16> userinfo_;
    push_back_string<32> host_;
    uint16_t port_;
    push_back_string<32> path_;
    push_back_string<32> query_;
    push_back_string<32> fragment_;
};

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_URI_HPP__
