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

class fixture
{
public:
    explicit fixture(const std::string & request);
    ~fixture();

    std::stringstream str_;
    rest::http::http_parser parser_;
};

fixture::fixture(const std::string & request)
    : str_(request)
    , parser_(std::bind((int(std::istream::*)()) &std::istream::get, &str_),
              std::bind(&std::istream::peek, &str_))
{
}

fixture::~fixture()
{
    EXPECT_EQ(parser_.valid(), parser_.lexer_.finished_);
    EXPECT_EQ(parser_.method(), parser_.data_.method_);
    EXPECT_EQ(parser_.version(), parser_.data_.version_);
    EXPECT_EQ(parser_.url(), parser_.data_.url_);
    EXPECT_EQ(parser_.status_code(), parser_.data_.status_code_);
    EXPECT_EQ(parser_.reason_phrase(), parser_.data_.reason_phrase_);
    EXPECT_EQ(parser_.headers(), parser_.data_.headers_);
}

TEST(http_parser, construction_destruction)
{
    fixture f("");
    EXPECT_EQ(f.parser_.lexer_.finished_, false);
    EXPECT_EQ(f.parser_.lexer_.last_character_, 0);
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_UNKNOWN);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_UNKNOWN);
    EXPECT_EQ(f.parser_.data_.url_.empty(), true);
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, options_request)
{
    fixture f("OPTIONS / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_OPTIONS);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_, "/");
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, get_request)
{
    fixture f("GET / HTTP/1.0\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_GET);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_0);
    EXPECT_EQ(f.parser_.data_.url_, "/");
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, head_request)
{
    fixture f("HEAD / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_HEAD);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_, "/");
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, post_request)
{
    fixture f("POST / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_POST);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_, "/");
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, put_request)
{
    fixture f("PUT /bla HTTP/1.1\r\nContent-Length:\n\t0\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_PUT);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_, "/bla");
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.size(), 1);
    EXPECT_EQ(f.parser_.data_.headers_.begin()->first, "content-length");
    EXPECT_EQ(f.parser_.data_.headers_.begin()->second, " 0");
}

TEST(http_parser, delete_request)
{
    fixture f("DELETE / HTTP/1.1\r\nContent-Length:\n 0\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_DELETE);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_, "/");
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.size(), 1);
    EXPECT_EQ(f.parser_.data_.headers_.begin()->first, "content-length");
    EXPECT_EQ(f.parser_.data_.headers_.begin()->second, " 0");
}

TEST(http_parser, trace_request)
{
    fixture f("TRACE / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_TRACE);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_, "/");
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, connect_request)
{
    fixture f("CONNECT / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_CONNECT);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_, "/");
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, gone_response)
{
    fixture f("HTTP/1.1 410 Gone\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_UNKNOWN);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_.empty(), true);
    EXPECT_EQ(f.parser_.data_.status_code_, 410);
    EXPECT_EQ(f.parser_.data_.reason_phrase_, "Gone");
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, not_found_response)
{
    fixture f("HTTP/1.1 404 Not Found\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_UNKNOWN);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_.empty(), true);
    EXPECT_EQ(f.parser_.data_.status_code_, 404);
    EXPECT_EQ(f.parser_.data_.reason_phrase_, "Not Found");
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, custom_response)
{
    fixture f("HTTP/1.1 555 X0Y1Z2\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_UNKNOWN);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_.empty(), true);
    EXPECT_EQ(f.parser_.data_.status_code_, 555);
    EXPECT_EQ(f.parser_.data_.reason_phrase_, "X0Y1Z2");
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}

TEST(http_parser, another_custom_response)
{
    fixture f("HTTP/1.1 555 9X0Y1Z2\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, true);
    EXPECT_EQ(f.parser_.lexer_.last_character_, '\n');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_UNKNOWN);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_HTTP_1_1);
    EXPECT_EQ(f.parser_.data_.url_.empty(), true);
    EXPECT_EQ(f.parser_.data_.status_code_, 555);
    EXPECT_EQ(f.parser_.data_.reason_phrase_, "9X0Y1Z2");
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}


TEST(http_parser, http_error)
{
    fixture f("abcdefghijklmnopqrstuvwxyz");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.lexer_.finished_, false);
    EXPECT_EQ(f.parser_.lexer_.last_character_, 'a');
    EXPECT_EQ(f.parser_.data_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.data_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.data_.method_, METHOD_UNKNOWN);
    EXPECT_EQ(f.parser_.data_.version_, VERSION_UNKNOWN);
    EXPECT_EQ(f.parser_.data_.url_.empty(), true);
    EXPECT_EQ(f.parser_.data_.status_code_, 0);
    EXPECT_EQ(f.parser_.data_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.data_.headers_.empty(), true);
}