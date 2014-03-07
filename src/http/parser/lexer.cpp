/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

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
#include <iostream>
#include <iomanip>
#include <map>

#include "lexer.h"

extern "C"
{

    int httpparse(void * scanner);

}

namespace rest
{

namespace http
{

namespace
{

char toLower(const char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return (c | 0x60);
    }
    return c;
}

}

HttpParser::HttpParser(const std::function<int()> & getFn, const std::function<int()> & peekFn)
    : m_getFn(getFn)
    , m_peekFn(peekFn)
    , m_finished(false)
    , m_lastChar(0)
    , m_headerKey()
    , m_headerValue()
    , m_method(METHOD_UNKNOWN)
    , m_version(VERSION_UNKNOWN)
    , m_url()
    , m_statusCode(0)
    , m_reasonPhrase()
    , m_headers()
{}

void HttpParser::parse()
{
    m_finished = false;
    m_headerKey.clear();
    m_headerValue.clear();
    httpparse(this);
}

void HttpParser::finished()
{
    m_finished = true;
}

void HttpParser::setHttpVerb(const HttpMethod & newMethod)
{
    m_method = newMethod;
}

void HttpParser::setHttpVersion(const HttpVersion & newVersion)
{
    m_version = newVersion;
}

void HttpParser::setStatusCode(uint16_t factor)
{
    uint16_t n = static_cast<uint16_t>((m_lastChar - '0') * factor);
    m_statusCode = static_cast<uint16_t>(m_statusCode + n);
}

void HttpParser::appendToUrl()
{
    m_url += toLower(m_lastChar);
}

void HttpParser::appendToReasonPhrase()
{
    m_reasonPhrase += m_lastChar;
}

void HttpParser::appendToHeaderKey()
{
    m_headerKey += toLower(m_lastChar);
}

void HttpParser::appendToHeaderValue()
{
    m_headerValue += toLower(m_lastChar);
}

void HttpParser::takeHeader()
{
    m_headers[m_headerKey] = m_headerValue;
    m_headerKey.clear();
    m_headerValue.clear();
}

int HttpParser::get()
{
    if (true == m_finished)
    {
        return -1;
    }

    int c = m_getFn();
    if (c == '\r')
    {
        if (m_peekFn() == '\n')
        {
            m_getFn();
        }
        c = '\n';
    }

    if (c == '\n')
    {
        if ((m_peekFn() == ' ') || (m_peekFn() == '\t'))
        {
            m_getFn();
            c = ' ';
        }
    }

    m_lastChar = static_cast<char>(c);
    return c;
}

void HttpParser::error(const char *)
{
}

bool HttpParser::valid() const
{
    return m_finished;
}

const HttpMethod & HttpParser::method() const
{
    return m_method;
}

const HttpVersion & HttpParser::version() const
{
    return m_version;
}

const std::string HttpParser::url() const
{
    return m_url;
}

const uint16_t & HttpParser::statusCode() const
{
    return m_statusCode;
}

const std::string HttpParser::reasonPhrase() const
{
    return m_reasonPhrase;
}

const std::map<std::string, std::string> & HttpParser::headers() const
{
    return m_headers;
}

} // namespace http

} // namespace rest



int httplex(int *, httpscan_t scanner)
{
    return static_cast<rest::http::HttpParser *>(scanner)->get();
}

void httperror(httpscan_t scanner, const char * s)
{
    static_cast<rest::http::HttpParser *>(scanner)->error(s);
}

void httpFinished(httpscan_t scanner)
{
    static_cast<rest::http::HttpParser *>(scanner)->finished();
}

void setHttpVerb(httpscan_t scanner, HttpMethod method)
{
    static_cast<rest::http::HttpParser *>(scanner)->setHttpVerb(method);
}

void setHttpVersion(httpscan_t scanner, HttpVersion version)
{
    static_cast<rest::http::HttpParser *>(scanner)->setHttpVersion(version);
}

void setStatusCode(httpscan_t scanner, uint16_t factor)
{
    static_cast<rest::http::HttpParser *>(scanner)->setStatusCode(factor);
}

void appendToUrl(httpscan_t scanner)
{
    static_cast<rest::http::HttpParser *>(scanner)->appendToUrl();
}

void appendToReasonPhrase(httpscan_t scanner)
{
    static_cast<rest::http::HttpParser *>(scanner)->appendToReasonPhrase();
}

void appendToHeaderKey(httpscan_t scanner)
{
    static_cast<rest::http::HttpParser *>(scanner)->appendToHeaderKey();
}

void appendToHeaderValue(httpscan_t scanner)
{
    static_cast<rest::http::HttpParser *>(scanner)->appendToHeaderValue();
}

void takeHeader(httpscan_t scanner)
{
    static_cast<rest::http::HttpParser *>(scanner)->takeHeader();
}

