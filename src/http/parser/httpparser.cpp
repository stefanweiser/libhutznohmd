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

#include <cmath>
#include <cstdint>
#include <map>

#include "httpparser.hpp"

extern "C"
{

    int httpparse(httpscan_t * scanner);

}

typedef struct httpscan {
    rest::http::Lexer * m_lexer;
    rest::http::Data * m_data;
} httpscan_t;

namespace rest
{

namespace http
{

HttpParser::HttpParser(const std::function<int()> & getFn,
                       const std::function<int()> & peekFn)
    : m_lexer(getFn, peekFn)
    , m_data()
    , m_httpscan(new httpscan {&m_lexer, &m_data})
{}

HttpParser::~HttpParser()
{
    delete m_httpscan;
}

void HttpParser::parse()
{
    if (false == m_lexer.finished()) {
        httpparse(m_httpscan);
    }
}

bool HttpParser::valid() const
{
    return m_lexer.finished();
}

const HttpMethod & HttpParser::method() const
{
    return m_data.method();
}

const HttpVersion & HttpParser::version() const
{
    return m_data.version();
}

const std::string HttpParser::url() const
{
    return m_data.url();
}

const uint16_t & HttpParser::statusCode() const
{
    return m_data.statusCode();
}

const std::string HttpParser::reasonPhrase() const
{
    return m_data.reasonPhrase();
}

const std::map<std::string, std::string> & HttpParser::headers() const
{
    return m_data.headers();
}

const size_t & HttpParser::contentLength() const
{
    return m_data.contentLength();
}

} // namespace http

} // namespace rest
