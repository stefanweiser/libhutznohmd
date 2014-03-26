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
#include <sstream>

#include "data.h"

namespace rest
{

namespace http
{

class Lexer;
class Data;

}

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

template <typename T>
T lexical_cast(const std::string & str)
{
    T var = T();
    std::istringstream iss;
    iss.str(str);
    iss >> var;
    return var;
}

} // namespace

Data::Data()
    : m_headerKey()
    , m_headerValue()
    , m_method(METHOD_UNKNOWN)
    , m_version(VERSION_UNKNOWN)
    , m_url()
    , m_statusCode(0)
    , m_reasonPhrase()
    , m_headers()
    , m_contentLength(0)
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
    HeaderType type = headerStringToEnum(m_headerKey);
    if (type == HeaderType::ContentLength) {
        m_contentLength = lexical_cast<size_t>(m_headerValue);
    }

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

const std::string & Data::url() const
{
    return m_url;
}

const uint16_t & Data::statusCode() const
{
    return m_statusCode;
}

const std::string & Data::reasonPhrase() const
{
    return m_reasonPhrase;
}

const std::map<std::string, std::string> & Data::headers() const
{
    return m_headers;
}

const size_t & Data::contentLength() const
{
    return m_contentLength;
}

HeaderType headerStringToEnum(const std::string & s)
{
    const static std::map<std::string, HeaderType> stringToEnumMap = {
        {"accept", HeaderType::Accept},
        {"accept-charset", HeaderType::AcceptCharset},
        {"accept-encoding", HeaderType::AcceptEncoding},
        {"accept-language", HeaderType::AcceptLanguage},
        {"accept-ranges", HeaderType::AcceptRanges},
        {"age", HeaderType::Age},
        {"allow", HeaderType::Allow},
        {"authorization", HeaderType::Authorization},
        {"cache-control", HeaderType::CacheControl},
        {"connection", HeaderType::Connection},
        {"content-encoding", HeaderType::ContentEncoding},
        {"content-language", HeaderType::ContentLanguage},
        {"content-length", HeaderType::ContentLength},
        {"content-location", HeaderType::ContentLocation},
        {"content-md5", HeaderType::ContentMD5},
        {"content-range", HeaderType::ContentRange},
        {"content-type", HeaderType::ContentType},
        {"cookie", HeaderType::Cookie},
        {"date", HeaderType::Date},
        {"etag", HeaderType::ETag},
        {"expect", HeaderType::Expect},
        {"expires", HeaderType::Expires},
        {"from", HeaderType::From},
        {"host", HeaderType::Host},
        {"if-match", HeaderType::IfMatch},
        {"if-modified-since", HeaderType::IfModifiedSince},
        {"if-none-match", HeaderType::IfNoneMatch},
        {"if-range", HeaderType::IfRange},
        {"if-unmodified-since", HeaderType::IfUnmodifiedSince},
        {"last-modified", HeaderType::LastModified},
        {"location", HeaderType::Location},
        {"max-forwards", HeaderType::MaxForwards},
        {"proxy-authenticate", HeaderType::ProxyAuthenticate},
        {"proxy-authorization", HeaderType::ProxyAuthorization},
        {"range", HeaderType::Range},
        {"referer", HeaderType::Referer},
        {"retry-after", HeaderType::RetryAfter},
        {"server", HeaderType::Server},
        {"te", HeaderType::TE},
        {"user-agent", HeaderType::UserAgent},
        {"vary", HeaderType::Vary},
        {"www-authenticate", HeaderType::WWWAuthenticate}
    };

    auto it = stringToEnumMap.find(s);
    if (it != stringToEnumMap.end()) {
        return it->second;
    }
    return HeaderType::Custom;
}

} // namespace http

} // namespace rest



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
