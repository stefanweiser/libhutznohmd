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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/parser/httpparser.hpp>

using namespace testing;

class Fixture
{
public:
    explicit Fixture(const std::string & req);
    ~Fixture();

    std::stringstream m_str;
    rest::http::HttpParser m_parser;
};

Fixture::Fixture(const std::string & req)
    : m_str(req)
    , m_parser(std::bind((int(std::istream::*)()) &std::istream::get, &m_str),
               std::bind(&std::istream::peek, &m_str))
{
}

Fixture::~Fixture()
{
    EXPECT_EQ(m_parser.valid(), m_parser.m_lexer.m_finished);
    EXPECT_EQ(m_parser.method(), m_parser.m_data.m_method);
    EXPECT_EQ(m_parser.version(), m_parser.m_data.m_version);
    EXPECT_EQ(m_parser.url(), m_parser.m_data.m_url);
    EXPECT_EQ(m_parser.statusCode(), m_parser.m_data.m_statusCode);
    EXPECT_EQ(m_parser.reasonPhrase(), m_parser.m_data.m_reasonPhrase);
    EXPECT_EQ(m_parser.headers(), m_parser.m_data.m_headers);
}

TEST(HttpParser, ConstructionDestruction)
{
    Fixture fixture("");
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, false);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_UNKNOWN);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_UNKNOWN);
    EXPECT_EQ(fixture.m_parser.m_data.m_url.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, OptionsRequest)
{
    Fixture fixture("OPTIONS / HTTP/1.1\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_OPTIONS);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url, "/");
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, GetRequest)
{
    Fixture fixture("GET / HTTP/1.0\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_GET);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_0);
    EXPECT_EQ(fixture.m_parser.m_data.m_url, "/");
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, HeadRequest)
{
    Fixture fixture("HEAD / HTTP/1.1\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_HEAD);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url, "/");
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, PostRequest)
{
    Fixture fixture("POST / HTTP/1.1\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_POST);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url, "/");
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, PutRequest)
{
    Fixture fixture("PUT /bla HTTP/1.1\r\nContent-Length:\n\t0\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_PUT);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url, "/bla");
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.size(), 1);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.begin()->first, "content-length");
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.begin()->second, " 0");
}

TEST(HttpParser, DeleteRequest)
{
    Fixture fixture("DELETE / HTTP/1.1\r\nContent-Length:\n 0\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_DELETE);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url, "/");
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.size(), 1);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.begin()->first, "content-length");
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.begin()->second, " 0");
}

TEST(HttpParser, TraceRequest)
{
    Fixture fixture("TRACE / HTTP/1.1\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_TRACE);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url, "/");
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, ConnectRequest)
{
    Fixture fixture("CONNECT / HTTP/1.1\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_CONNECT);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url, "/");
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, GoneResponse)
{
    Fixture fixture("HTTP/1.1 410 Gone\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_UNKNOWN);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 410);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase, "Gone");
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, NotFoundResponse)
{
    Fixture fixture("HTTP/1.1 404 Not Found\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_UNKNOWN);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 404);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase, "Not Found");
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, CustomResponse)
{
    Fixture fixture("HTTP/1.1 555 X0Y1Z2\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_UNKNOWN);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 555);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase, "X0Y1Z2");
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}

TEST(HttpParser, Custom2Response)
{
    Fixture fixture("HTTP/1.1 555 9X0Y1Z2\r\n\r\n");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, true);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, '\n');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_UNKNOWN);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_HTTP_1_1);
    EXPECT_EQ(fixture.m_parser.m_data.m_url.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 555);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase, "9X0Y1Z2");
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}


TEST(HttpParser, HttpError)
{
    Fixture fixture("abcdefghijklmnopqrstuvwxyz");
    fixture.m_parser.parse();
    EXPECT_EQ(fixture.m_parser.m_lexer.m_finished, false);
    EXPECT_EQ(fixture.m_parser.m_lexer.m_lastChar, 'a');
    EXPECT_EQ(fixture.m_parser.m_data.m_headerKey.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headerValue.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_method, METHOD_UNKNOWN);
    EXPECT_EQ(fixture.m_parser.m_data.m_version, VERSION_UNKNOWN);
    EXPECT_EQ(fixture.m_parser.m_data.m_url.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_statusCode, 0);
    EXPECT_EQ(fixture.m_parser.m_data.m_reasonPhrase.empty(), true);
    EXPECT_EQ(fixture.m_parser.m_data.m_headers.empty(), true);
}
