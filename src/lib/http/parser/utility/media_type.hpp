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

#ifndef LIBHUTZNOHMD_HTTP_PARSER_UTILITY_MEDIA_TYPE_HPP
#define LIBHUTZNOHMD_HTTP_PARSER_UTILITY_MEDIA_TYPE_HPP

#include <cstddef>
#include <cstring>
#include <map>

#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/push_back_string.hpp>

#include <hutzn.hpp>

namespace hutzn
{

namespace http
{

//! ...
class media_type : public media_type_interface
{
public:
    //! ...
    explicit media_type();

    //! ...
    media_type(const media_type& rhs);

    //! ...
    media_type& operator=(const media_type& rhs);

    //! ...
    bool parse(const lexer& lex, int32_t& character);

    //! ...
    media_type_interface::mime_type type() const override;

    //! ...
    media_type_interface::mime_subtype subtype() const override;

    //! ...
    const char_t* custom_type() const override;

    //! ...
    const char_t* custom_subtype() const override;

    //! ...
    const std::map<std::string, std::string>& parameters() const override;

    //! ...
    uint8_t quality() const override;

    //! ...
    uint8_t specification_grade() const override;

private:
    void parse_type(int32_t& character);
    void parse_subtype(int32_t& character);
    bool parse_parameter(int32_t& character);

    bool parse_quality_parameter(int32_t& character);

    const lexer* lexer_;
    media_type_interface::mime_type type_;
    media_type_interface::mime_subtype subtype_;
    push_back_string<32> custom_type_;
    push_back_string<64> custom_subtype_;
    std::map<std::string, std::string> parameters_;

    uint8_t quality_;
};

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HTTP_PARSER_UTILITY_MEDIA_TYPE_HPP
