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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_MEDIATYPE_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_MEDIATYPE_HPP__

#include <cstddef>
#include <cstring>
#include <map>

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
    WILDCARD = 0,
    PLAIN = 1
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
    const char * parameter(const char * key) const;
    uint8_t quality() const;

    //! Returns a value, that defines how specific the media type is. The less the value, the more
    //! unspecific the media type is. These values can be used to compare media types with each
    //! other.
    uint8_t specification_grade() const;

private:
    void parse_type(int32_t & character);
    void parse_subtype(int32_t & character);
    bool parse_parameter(int32_t & character);

    bool parse_quality_parameter(int32_t & character);

    const lexer & lexer_;
    media_type_type type_;
    media_type_subtype subtype_;
    push_back_string<32> custom_type_;
    push_back_string<64> custom_subtype_;
    std::map<std::string, std::string> parameters_;

    uint8_t quality_;
};

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_MEDIATYPE_HPP__
