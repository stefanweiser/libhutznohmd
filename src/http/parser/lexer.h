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

#ifndef __LIBREST_HTTP_PARSER_LEXER_H__
#define __LIBREST_HTTP_PARSER_LEXER_H__

#ifdef __cplusplus
#include <functional>
#include <map>

extern "C"
{
#endif

typedef void * httpscan_t;

typedef enum {
    METHOD_UNKNOWN = 0,
    METHOD_HEAD = 1,
    METHOD_GET = 2,
    METHOD_PUT = 3,
    METHOD_DELETE = 4,
    METHOD_POST = 5,
    METHOD_TRACE = 6,
    METHOD_OPTIONS = 7,
    METHOD_CONNECT = 8
} HttpMethod;

typedef enum {
    VERSION_UNKNOWN = 0,
    VERSION_HTTP_1_0 = 1,
    VERSION_HTTP_1_1 = 2
} HttpVersion;

int httplex(int * httplval, httpscan_t scanner);
void httperror(httpscan_t scanner, const char * s);

void httpFinished(httpscan_t scanner);
void setHttpVerb(httpscan_t scanner, HttpMethod method);
void setHttpVersion(httpscan_t scanner, HttpVersion version);
void setStatusCode(httpscan_t scanner, uint16_t factor);
void appendToUrl(httpscan_t scanner);
void appendToReasonPhrase(httpscan_t scanner);
void appendToHeaderKey(httpscan_t scanner);
void appendToHeaderValue(httpscan_t scanner);
void takeHeader(httpscan_t scanner);

#ifdef __cplusplus
}

namespace rest
{

namespace http
{

enum class HeaderType
{
    Custom,
    Accept,
    AcceptCharset,
    AcceptEncoding,
    AcceptLanguage,
    Authorization,
    Expect,
    From,
    Host,
    IfMatch,
    IfModifiedSince,
    IfNoneMatch,
    IfRange,
    IfUnmodifiedSince,
    MaxForwards,
    ProxyAuthorization,
    Range,
    Referer,
    TE,
    UserAgent,
    AcceptRanges,
    Age,
    ETag,
    Location,
    ProxyAuthenticate,
    RetryAfter,
    Server,
    Vary,
    WWWAuthenticate,
    Allow,
    ContentEncoding,
    ContentLanguage,
    ContentLength,
    ContentLocation,
    ContentMD5,
    ContentRange,
    ContentType,
    Expires,
    LastModified
};

class HttpParser
{
public:
    explicit HttpParser(const std::function<int()> & getFn, const std::function<int()> & peekFn);
    void parse();
    void finished();
    void setHttpVerb(const HttpMethod & newMethod);
    void setHttpVersion(const HttpVersion & newVersion);
    void setStatusCode(uint16_t factor);
    void appendToUrl();
    void appendToReasonPhrase();
    void appendToHeaderKey();
    void appendToHeaderValue();
    void takeHeader();
    int get();
    void error(const char * s);
    bool valid() const;
    const HttpMethod & method() const;
    const HttpVersion & version() const;
    const std::string url() const;
    const uint16_t & statusCode() const;
    const std::string reasonPhrase() const;
    const std::map<std::string, std::string> & headers() const;

private:
    std::function<int()> m_getFn;
    std::function<int()> m_peekFn;
    bool m_finished;
    char m_lastChar;
    std::string m_headerKey;
    std::string m_headerValue;

    HttpMethod m_method;
    HttpVersion m_version;
    std::string m_url;
    uint16_t m_statusCode;
    std::string m_reasonPhrase;
    std::map<std::string, std::string> m_headers;
};

} // namespace http

} // namespace rest
#endif

#endif /* __LIBREST_HTTP_PARSER_LEXER_H__ */
