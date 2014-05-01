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

#ifndef __LIBREST_HTTP_PARSER_HTTPPARSER_HPP__
#define __LIBREST_HTTP_PARSER_HTTPPARSER_HPP__

#include <http/parser/http.hpp>

namespace rest
{

namespace http
{

class http_parser
{
public:
    explicit http_parser(const anonymous_int_function & get_functor,
                         const anonymous_int_function & peek_functor);
    void parse();
    bool valid() const;
    const rest::http::method & method() const;
    const rest::http::version & version() const;
    const std::string url() const;
    const uint16_t & status_code() const;
    const std::string reason_phrase() const;
    const std::string header(const std::string & key) const;
    const size_t & content_length() const;
    const time_t & date() const;
    bool is_keep_connection() const;

private:
    httpscan httpscan_;
};

} // namespace http

} // namespace rest

#endif /* __LIBREST_HTTP_PARSER_HTTPPARSER_HPP__ */
