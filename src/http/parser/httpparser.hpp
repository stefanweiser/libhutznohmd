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

#ifndef __LIBREST_HTTP_PARSER_HTTPPARSER_HPP__
#define __LIBREST_HTTP_PARSER_HTTPPARSER_HPP__

#include <http/parser/httpscan.h>
#include <http/parser/data.h>
#include <http/parser/lexer.h>

namespace rest
{

namespace http
{

class HttpParser
{
public:
    explicit HttpParser(const std::function<int()> & getFn, const std::function<int()> & peekFn);
    ~HttpParser();
    void parse();
    bool valid() const;
    const HttpMethod & method() const;
    const HttpVersion & version() const;
    const std::string url() const;
    const uint16_t & statusCode() const;
    const std::string reasonPhrase() const;
    const std::map<std::string, std::string> & headers() const;
    const size_t & contentLength() const;

private:
    HttpParser(const HttpParser & parser) = delete;
    HttpParser & operator=(const HttpParser & parser) = delete;

    Lexer m_lexer;
    Data m_data;
    httpscan_t * m_httpscan;
};

} // namespace http

} // namespace rest

#endif /* __LIBREST_HTTP_PARSER_HTTPPARSER_HPP__ */
