/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_HTTP_PARSER_RESPONSE_PARSER_HPP
#define LIBHUTZNOHMD_HTTP_PARSER_RESPONSE_PARSER_HPP

#include <http/parser/utility/anonymous_int_function.hpp>
#include <http/parser/utility/base_parser.hpp>
#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/media_type.hpp>
#include <http/parser/utility/push_back_string.hpp>

#include <hutzn.hpp>

namespace hutzn
{

namespace http
{

class response_parser
{
public:
    //! Constructs the response parser.
    //! @param get_functor Returns an integer with the current character in the
    //! input stream and sets the pointer of the current character one byte
    //! further. This value must be in [0..255] for valid and less than 0 for
    //! invalid character.
    //! @param peek_functor Returns an integer with the current character in
    //! the input stream. This value must be in [0..255] for valid and less
    //! than 0 for invalid character.
    explicit response_parser(const anonymous_int_function& get_functor,
                             const anonymous_int_function& peek_functor);
    void parse();
    bool valid() const;
    const hutzn::http::version& version() const;
    const std::map<std::string, std::string>& headers() const;
    const size_t& content_length() const;
    const media_type_interface& content_type() const;
    const time_t& date() const;
    bool keeps_connection() const;
    const std::array<uint8_t, 16>& md5() const;
    bool has_md5() const;
    const uint16_t& status_code() const;
    const char* reason_phrase() const;

private:
    bool parse_connection(int32_t& character);
    bool parse_content_length(int32_t& character);
    bool parse_content_md5(int32_t& character);
    bool parse_content_type(int32_t& character);
    bool parse_date(int32_t& character);
    bool parse_headers(int32_t& character);

    base_parser common_;
    uint16_t status_code_;
    push_back_string<100> reason_phrase_;
};

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HTTP_PARSER_RESPONSE_PARSER_HPP
