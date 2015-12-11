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

#include <cstddef>

#include "libhutznohmd/types.hpp"

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

    bool parse(const char_t* source, size_t source_length, char_t*& destination,
               size_t& destination_length, bool skip_scheme = false);

    const uri_scheme& scheme(void) const;
    const char_t* userinfo(void) const;
    const char_t* host(void) const;
    const uint16_t& port(void) const;
    const char_t* path(void) const;
    const char_t* query(void) const;
    const char_t* fragment(void) const;

private:
    //! Contains data of the first pass.
    struct first_pass_data
    {
        //! Points to the begin of the scheme.
        const char_t* scheme{NULL};

        //! Contains the size of the scheme.
        size_t scheme_size{0};

        //! Points to the begin of the authority.
        char_t* authority{NULL};

        //! Contains the size of the authority.
        size_t authority_size{0};

        //! Points to the begin of the path.
        const char_t* path{NULL};

        //! Contains the size of the path.
        size_t path_size{0};

        //! Points to the begin of the query.
        const char_t* query{NULL};

        //! Contains the size of the query.
        size_t query_size{0};

        //! Points to the begin of the fragment.
        const char_t* fragment{NULL};

        //! Contains the size of the fragment.
        size_t fragment_size{0};
    };

    bool parse_1st_pass(const char_t* source, size_t source_length,
                        char_t*& destination, size_t& destination_length,
                        first_pass_data& data, bool skip_scheme);
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
    friend class uri_test_first_pass_empty_Test;
    friend class uri_test_first_pass_simple_path_Test;
    friend class uri_test_first_pass_localhost_Test;
    friend class uri_test_first_pass_complete_uri_Test;
    friend class uri_test_first_pass_only_authority_Test;
    friend class uri_test_first_pass_only_authority_with_userinfo_Test;
    friend class uri_test_first_pass_only_scheme_Test;
    friend class uri_test_first_pass_no_authority_Test;
    friend class uri_test_first_pass_only_query_Test;
    friend class uri_test_first_pass_only_fragment_Test;
    friend class uri_test_first_pass_encoded_characters_in_scheme_Test;
    friend class uri_test_first_pass_encoded_characters_in_authority_Test;
    friend class uri_test_first_pass_encoded_characters_in_authority_2_Test;
    friend class uri_test_first_pass_encoded_characters_in_path_Test;
    friend class uri_test_first_pass_encoded_characters_in_query_Test;
    friend class uri_test_first_pass_encoded_characters_in_fragment_Test;
    friend class uri_test_first_pass_encoded_characters_at_the_end_Test;
    friend class uri_test_valid_minimal_uri_Test;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_URI_HPP
