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

//! @brief Implements parsing of URIs as specified in RFC 3986.
//!
//! It supports only the specified schemes.
class uri
{
public:
    //! @brief Initializes all members.
    //!
    //! Call @ref hutzn::uri::parse() to set them.
    explicit uri(void);

    explicit uri(const uri& rhs) = delete;
    uri& operator=(const uri& rhs) = delete;

    //! @brief Parses an uri string.
    //!
    //! Disassembles the uri string, by nullterminating its parts and parsing
    //! the scheme into an enum value. The disassembled parts may need at most
    //! 2 bytes more of space, than the original source data. Therefore a
    //! destination string buffer has to be provided, that must be at least 2
    //! bytes larger than the uri string. The buffer data can overlap as long as
    //! the destination buffer begins before the source buffer.
    //! @warning The callee has to ensure, that the destination buffer is not
    //! freed before the uri!
    //! @param[in] source             Points to the uri string.
    //! @param[in] source_length      Length of the uri string in bytes.
    //! @param[in] destination        Points to the destination buffer.
    //! @param[in] destination_length Length of the destination buffer.
    //! @param[in] skip_scheme        Assumes that there is no scheme when true.
    //! @return                       True when the uri was successfully parsed
    //!                               and false if not.
    bool parse(const char_t* source, size_t source_length, char_t* destination,
               size_t destination_length, const bool skip_scheme = false);

    //! @brief Returns the scheme of the uri.
    //!
    //! Is valid after parse was called.
    //! @return Scheme of the uri.
    uri_scheme scheme(void) const;

    //! @brief Returns the userinfo of the uri.
    //!
    //! Is valid after parse was called.
    //! @return Userinfo of the uri.
    const char_t* userinfo(void) const;

    //! @brief Returns the host of the uri.
    //!
    //! Is valid after parse was called.
    //! @return Host of the uri.
    const char_t* host(void) const;

    //! @brief Returns the port of the uri.
    //!
    //! Is valid after parse was called.
    //! @return Port of the uri.
    uint16_t port(void) const;

    //! @brief Returns the path of the uri.
    //!
    //! Is valid after parse was called.
    //! @return Path of the uri.
    const char_t* path(void) const;

    //! @brief Returns the query string of the uri.
    //!
    //! Is valid after parse was called.
    //! @return Query string of the uri.
    const char_t* query(void) const;

    //! @brief Returns the fragment of the uri.
    //!
    //! Is valid after parse was called.
    //! @return Fragment of the uri.
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

    //! @brief Parses the uri a first time.
    //!
    //! Rewrites the uri by splitting its parts and fill a first_pass_data
    //! structure.
    //! @param[in] source                 Points to the encoded uri.
    //! @param[in] source_length          Length of the encoded uri in bytes.
    //! @param[in,out] destination        Points to a buffer where to write to.
    //! @param[in,out] destination_length Length of the destination buffer.
    //! @param[in,out] data               Reference to the first_pass_data.
    //! @param[in] skip_scheme            Skips the scheme when true.
    //! @return                           True if success of false when failed.
    bool parse_1st_pass(const char_t* source, size_t source_length,
                        char_t* destination, size_t destination_length,
                        first_pass_data& data, const bool skip_scheme);

    //! @brief Parses the scheme of an uri.
    //!
    //! Converts the scheme of the first pass into a value of the enum class
    //! @ref uri_scheme.
    //! @param[in] scheme_ptr Points to the begin of the scheme.
    //! @param[in] size       Size of the scheme string in bytes.
    //! @return               True if a valid scheme was found.
    bool parse_scheme(const char_t* const scheme_ptr, const size_t& size);

    //! @brief Parses the authority of an uri.
    //!
    //! Splits the authority part into userinfo, host and port.
    //! @param[in] authority_ptr Points to the begin of the authority.
    //! @param[in] size          Size of the authority string in bytes.
    //! @return                  True if the split and conversion succeeded.
    bool parse_authority(char_t* const authority_ptr, const size_t& size);

    //! Scheme of the uri.
    uri_scheme scheme_;

    //! Points to an external buffer containing the userinfo.
    const char_t* userinfo_;

    //! Points to an external buffer containing the host.
    const char_t* host_;

    //! Contains the parsed port or 0.
    uint16_t port_;

    //! Points to an external buffer containing the path.
    const char_t* path_;

    //! Points to an external buffer containing the query string.
    const char_t* query_;

    //! Points to an external buffer containing the fragment.
    const char_t* fragment_;

    friend class uri_test;
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
