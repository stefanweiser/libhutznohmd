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

#ifndef __LIBREST_HTTP_PARSER_REQUESTPARSER_HPP__
#define __LIBREST_HTTP_PARSER_REQUESTPARSER_HPP__

#include <http/parser/utility/anonymousintfunction.hpp>
#include <http/parser/utility/baseparser.hpp>
#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/mediatype.hpp>
#include <http/parser/utility/pushbackstring.hpp>

#include <rest.hpp>

namespace rest
{

namespace http
{

class request_parser: public base_parser
{
public:
    //! Constructs the request parser.
    //! @param get_functor Returns an integer with the current character in the input stream and
    //!                    sets the pointer of the current character one byte further. This value
    //!                    must be in [0..255] for valid and less than 0 for invalid character.
    //! @param peek_functor Returns an integer with the current character in the input stream.
    //!                     This value must be in [0..255] for valid and less than 0 for invalid
    //!                     character.
    explicit request_parser(const anonymous_int_function & get_functor,
                            const anonymous_int_function & peek_functor);
    void parse();
    const rest::http::method & method() const;
    const std::string request_uri() const;

private:
    bool parse_accept(int32_t & character);
    bool parse_headers(int32_t & character);

    rest::http::method method_;
    push_back_string<1000> request_uri_;

    std::vector<std::unique_ptr<media_type>> accept_header_;
};

} // namespace http

} // namespace rest

#endif /* __LIBREST_HTTP_PARSER_REQUESTPARSER_HPP__ */