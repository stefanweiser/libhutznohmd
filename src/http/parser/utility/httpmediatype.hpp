/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBREST_HTTP_PARSER_UTILITY_HTTPMEDIATYPE_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_HTTPMEDIATYPE_HPP__

#include <cstddef>
#include <cstring>

#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/pushbackstring.hpp>

namespace rest
{

namespace http
{

enum class media_type_type : int32_t
{
    CUSTOM = -1,
    WILDCARD = 0,
    APPLICATION = 1,
    AUDIO = 2,
    EXAMPLE = 3,
    IMAGE = 4,
    MESSAGE = 5,
    MODEL = 6,
    MULTIPART = 7,
    TEXT = 8,
    VIDEO = 9
};

enum class media_type_subtype : int32_t
{
    CUSTOM = -1,
    WILDCARD = 0
};

class media_type
{
public:
    explicit media_type(const lexer & l);

    bool parse(int32_t & character);

    media_type_type type() const;
    media_type_subtype subtype() const;
    const char * custom_type() const;
    const char * custom_subtype() const;

private:
    template<typename lower_case_string>
    bool finish_known(lower_case_string, int32_t & character);

    bool parse_type_end(int32_t & character);
    bool parse_type_custom(int32_t & character);
    bool parse_type_wildcard(int32_t & character);
    bool parse_type_application(int32_t & character);
    bool parse_type_audio(int32_t & character);
    bool parse_type_example(int32_t & character);
    bool parse_type_image(int32_t & character);
    bool parse_type_message(int32_t & character);
    bool parse_type_model(int32_t & character);
    bool parse_type_multipart(int32_t & character);
    bool parse_type_text(int32_t & character);
    bool parse_type_video(int32_t & character);
    bool parse_type_a(int32_t & character);
    bool parse_type_m(int32_t & character);

    bool parse_subtype_wildcard(int32_t & character);

    bool parse_type(int32_t & character);
    bool parse_subtype(int32_t & character);

    const lexer & lexer_;
    media_type_type type_;
    media_type_subtype subtype_;
    push_back_string<32> custom_type_;
    push_back_string<64> custom_subtype_;
};

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_HTTPMEDIATYPE_HPP__
