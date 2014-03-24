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

#include "lexer.h"

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

namespace
{

char toLower(const char c)
{
    if (c >= 'A' && c <= 'Z') {
        return (c | 0x60);
    }
    return c;
}

}

Lexer::Lexer(const std::function<int()> & getFn, const std::function<int()> & peekFn)
    : m_getFn(getFn)
    , m_peekFn(peekFn)
    , m_lastChar(0)
    , m_finished(false)
{}

void Lexer::finish()
{
    m_finished = true;
}

bool Lexer::finished() const
{
    return m_finished;
}

void Lexer::error(const char * /*s*/)
{
}

int Lexer::get()
{
    if (true == m_finished) {
        return -1;
    }

    int c = m_getFn();
    if (c == '\r') {
        if (m_peekFn() == '\n') {
            m_getFn();
        }
        c = '\n';
    }

    if ((c == '\n') && (m_lastChar != '\n')) {
        if ((m_peekFn() == ' ') || (m_peekFn() == '\t')) {
            m_getFn();
            c = ' ';
        }
    }

    m_lastChar = static_cast<char>(c);
    return c;
}

Data::Data()
    : m_headerKey()
    , m_headerValue()
    , m_method(METHOD_UNKNOWN)
    , m_version(VERSION_UNKNOWN)
    , m_url()
    , m_statusCode(0)
    , m_reasonPhrase()
    , m_headers()
{
}

void Data::setHttpVerb(const HttpMethod & newMethod)
{
    m_method = newMethod;
}

void Data::setHttpVersion(const HttpVersion & newVersion)
{
    m_version = newVersion;
}

void Data::setStatusCode(uint16_t factor, char token)
{
    uint16_t n = static_cast<uint16_t>((token - '0') * factor);
    m_statusCode = static_cast<uint16_t>(m_statusCode + n);
}

void Data::appendToUrl(char token)
{
    m_url += toLower(token);
}

void Data::appendToReasonPhrase(char token)
{
    m_reasonPhrase += token;
}

void Data::appendToHeaderKey(char token)
{
    m_headerKey += toLower(token);
}

void Data::appendToHeaderValue(char token)
{
    m_headerValue += token;
}

void Data::takeHeader()
{
    m_headers[m_headerKey] = m_headerValue;
    m_headerKey.clear();
    m_headerValue.clear();
}

const HttpMethod & Data::method() const
{
    return m_method;
}

const HttpVersion & Data::version() const
{
    return m_version;
}

const std::string Data::url() const
{
    return m_url;
}

const uint16_t & Data::statusCode() const
{
    return m_statusCode;
}

const std::string Data::reasonPhrase() const
{
    return m_reasonPhrase;
}

const std::map<std::string, std::string> & Data::headers() const
{
    return m_headers;
}

HttpParser::HttpParser(const std::function<int()> & getFn, const std::function<int()> & peekFn)
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

} // namespace http

} // namespace rest



int httplex(int * lval, httpscan_t * scanner)
{
    int result = scanner->m_lexer->get();
    *lval = result;
    return result;
}

void httperror(httpscan_t * scanner, const char * s)
{
    scanner->m_lexer->error(s);
}

void httpFinish(httpscan_t * scanner)
{
    scanner->m_lexer->finish();
}

void setHttpVerb(httpscan_t * scanner, HttpMethod method)
{
    scanner->m_data->setHttpVerb(method);
}

void setHttpVersion(httpscan_t * scanner, HttpVersion version)
{
    scanner->m_data->setHttpVersion(version);
}

void setStatusCode(httpscan_t * scanner, uint16_t factor, char token)
{
    scanner->m_data->setStatusCode(factor, token);
}

void appendToUrl(httpscan_t * scanner, char token)
{
    scanner->m_data->appendToUrl(token);
}

void appendToReasonPhrase(httpscan_t * scanner, char token)
{
    scanner->m_data->appendToReasonPhrase(token);
}

void appendToHeaderKey(httpscan_t * scanner, char token)
{
    scanner->m_data->appendToHeaderKey(token);
}

void appendToHeaderValue(httpscan_t * scanner, char token)
{
    scanner->m_data->appendToHeaderValue(token);
}

void takeHeader(httpscan_t * scanner)
{
    scanner->m_data->takeHeader();
}

