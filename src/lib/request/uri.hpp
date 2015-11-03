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

#include "hutzn.hpp"

namespace hutzn
{

//! These URI schemes are supported by the parser and needed for HTTP.
enum class uri_scheme : uint8_t {
    //! Scheme is not known or invalid.
    UNKNOWN = 0,

    //! URI describes a URL as used in HTTP for plain data transport.
    HTTP = 1,

    //! URI describes a URL as used in HTTP within an encrypted channel.
    HTTPS = 2
};

//! Implements parsing of URIs as specified in RFC 3986. It supports only the
//! specified schemes.
class uri
{
public:
    explicit uri(void);

    explicit uri(const uri& rhs) = delete;
    uri& operator=(const uri& rhs) = delete;

    bool parse(char_t*& data, size_t& remaining, bool skip_scheme = false);

    const uri_scheme& scheme(void) const;
    const char_t* userinfo(void) const;
    const char_t* host(void) const;
    const uint16_t& port(void) const;
    const char_t* path(void) const;
    const char_t* query(void) const;
    const char_t* fragment(void) const;

    class first_pass_data
    {
    public:
        explicit first_pass_data(void);

        const char_t* scheme;
        size_t scheme_size;

        char_t* authority;
        size_t authority_size;

        const char_t* path;
        size_t path_size;

        const char_t* query;
        size_t query_size;

        const char_t* fragment;
        size_t fragment_size;
    };

private:
    bool parse_1st_pass(char_t*& raw, size_t& remaining, first_pass_data& data,
                        bool skip_scheme);
    bool parse_scheme(const char_t* const scheme_ptr, const size_t& size);
    bool parse_authority(char_t* const authority_ptr, const size_t& size);

    bool already_called_;
    uri_scheme scheme_;
    const char_t* userinfo_;
    const char_t* host_;
    uint16_t port_;
    const char_t* path_;
    const char_t* query_;
    const char_t* fragment_;

    friend class uri_test;
    friend class uri_test_first_pass_NULL_Test;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_URI_HPP
