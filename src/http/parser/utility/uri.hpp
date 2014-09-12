/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREST_HTTP_PARSER_UTILITY_URI_HPP
#define LIBREST_HTTP_PARSER_UTILITY_URI_HPP

#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/push_back_string.hpp>

#include <rest.hpp>

namespace rest
{

namespace http
{

//! Implements parsing of URIs as specified in RFC 3986. It supports only the specified schemes.
class uri: public uri_interface
{
public:
    explicit uri();

    explicit uri(const uri & rhs) = delete;
    uri & operator=(const uri & rhs) = delete;

    bool parse(const lexer & l, int32_t & character, const bool skip_scheme);

    void set_scheme(const uri_scheme & new_scheme);
    bool set_userinfo(const char * new_userinfo);
    bool set_host(const char * new_host);
    void set_port(const uint16_t & new_port);

    virtual bool valid() const;
    virtual const uri_scheme & scheme() const;
    virtual const char * userinfo() const;
    virtual const char * host() const;
    virtual const uint16_t & port() const;
    virtual const char * path() const;
    virtual const char * query() const;
    virtual const char * fragment() const;

private:
    bool parse_scheme_and_authority(int32_t & character, const bool skip_scheme);
    bool parse_scheme(int32_t & character);
    bool parse_userinfo_and_authority(int32_t & character);
    bool parse_authority(int32_t & character);
    bool parse_authority_1st_pass(int32_t & character);
    bool parse_authority_2nd_pass();

    bool parse_word();

    const lexer * lexer_;
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

} // namespace rest

#endif // LIBREST_HTTP_PARSER_UTILITY_URI_HPP
