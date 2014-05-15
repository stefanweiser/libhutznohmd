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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_BASEPARSER_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_BASEPARSER_HPP__

#include <http/parser/utility/anonymousintfunction.hpp>
#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/mediatype.hpp>
#include <http/parser/utility/pushbackstring.hpp>

#include <rest.hpp>

namespace rest
{

namespace http
{

enum class parser_state
{
    UNFINISHED = 0,
    SUCCEEDED = 1,
    ERROR = 2
};

enum class connection_type
{
    ERROR = 0,
    CLOSE,
    KEEP_ALIVE
};

class base_parser
{
public:
    explicit base_parser(const anonymous_int_function & get_functor,
                         const anonymous_int_function & peek_functor);
    virtual ~base_parser();
    bool valid() const;
    const rest::http::version & version() const;
    const std::string header(const std::string & key) const;
    const size_t & content_length() const;
    const time_t & date() const;
    bool is_keep_connection() const;

protected:
    bool parse_connection(int32_t & result);
    bool parse_content_length(int32_t & result);
    bool parse_content_type(int32_t & result);
    bool parse_date(int32_t & result);
    bool parse_header(int32_t & result);
    bool parse_headers(int32_t & result);

    lexer lexer_;
    parser_state state_;

    push_back_string<40> header_key_;
    push_back_string<1000> header_value_;
    rest::http::version version_;
    std::map<std::string, std::string> headers_;
    size_t content_length_;
    media_type content_type_;
    time_t date_;
    connection_type connection_;
};

} // namespace http

} // namespace rest

#endif /* __LIBREST_HTTP_PARSER_UTILITY_BASEPARSER_HPP__ */
