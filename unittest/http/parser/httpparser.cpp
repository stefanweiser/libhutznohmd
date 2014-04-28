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

namespace rest
{

namespace http
{

namespace
{

typedef std::pair<std::string, size_t> string_index_pair;

int32_t get_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return p->first[p->second++];
    }
    return -1;
}

int32_t peek_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return p->first[p->second];
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string & request);
    ~fixture();

    string_index_pair str_;
    http_parser parser_;
};

fixture::fixture(const std::string & request)
    : str_(std::make_pair(request, 0))
    , parser_(anonymous_int_function(&get_char, &str_),
              anonymous_int_function(&peek_char, &str_))
{}

fixture::~fixture()
{
    EXPECT_EQ(parser_.valid(), (parser_state::SUCCEEDED == parser_.httpscan_.state_));
    EXPECT_EQ(parser_.method(), parser_.httpscan_.method_);
    EXPECT_EQ(parser_.version(), parser_.httpscan_.version_);
    EXPECT_EQ(parser_.url(), parser_.httpscan_.url_.c_str());
    EXPECT_EQ(parser_.status_code(), parser_.httpscan_.status_code_);
    EXPECT_EQ(parser_.reason_phrase(), parser_.httpscan_.reason_phrase_.c_str());
}

} // namespace

TEST(http_parser, construction_destruction)
{
    fixture f("");
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::UNFINISHED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, 0);
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
}

TEST(http_parser, options_request)
{
    fixture f("OPTIONS / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::OPTIONS);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
}

TEST(http_parser, get_request)
{
    fixture f("GET / HTTP/1.0\r\r\n");
    for (size_t i = 0; i < 2; i++) {
        f.parser_.parse();
        EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
        EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
        EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
        EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
        EXPECT_EQ(f.parser_.httpscan_.method_, method::GET);
        EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_0);
        EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
        EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
        EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
        EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
    }
}

TEST(http_parser, head_request)
{
    fixture f("HEAD / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::HEAD);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
}

TEST(http_parser, post_request)
{
    fixture f("POST / HTTP/1.1\r\nContent-Type: text/*\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::POST);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.content_type_.type(), media_type_type::TEXT);
    EXPECT_EQ(f.parser_.httpscan_.content_type_.subtype(), media_type_subtype::WILDCARD);
}

TEST(http_parser, put_request)
{
    fixture f("PUT /bla HTTP/1.1\r\nContent-Length:\n\t1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::PUT);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/bla"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.content_length(), 1);
    EXPECT_EQ(f.parser_.header("???"), "");
}

TEST(http_parser, delete_request)
{
    fixture f("DELETE / HTTP/1.1\r\nContent-Length:\n 1\r\nABC:\r\nDEF:\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::DELETE);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 2);
    EXPECT_EQ(f.parser_.content_length(), 1);
    EXPECT_EQ(f.parser_.header("abc"), "");
}

TEST(http_parser, trace_request)
{
    fixture f("TRACE / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::TRACE);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
}

TEST(http_parser, connect_request)
{
    fixture f("CONNECT / HTTP/1.1\r\nContent-Length2: 5\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::CONNECT);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 1);
}

TEST(http_parser, same_header_name_request)
{
    fixture f("GET / HTTP/1.0\r\nAllow: HEAD\r\nAllow: GET\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::GET);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_0);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 1);
    EXPECT_EQ(f.parser_.header("allow"), " HEAD, GET");
}

TEST(http_parser, same_custom_header_name_request)
{
    fixture f("GET / HTTP/1.0\r\nCheck: HEAD\r\nCheck: GET\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::GET);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_0);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 1);
    EXPECT_EQ(f.parser_.header("check"), " HEAD, GET");
}

TEST(http_parser, unknown_content_length_in_request)
{
    fixture f("DELETE / HTTP/1.1\r\nContent-Length:\n a\r\nABC:\r\nDEF:\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, 'a');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::DELETE);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 0);
    EXPECT_EQ(f.parser_.content_length(), 0);
}

TEST(http_parser, unknown_date_in_request)
{
    fixture f("DELETE / HTTP/1.1\r\nDate:\n a\r\nABC:\r\nDEF:\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, 'B');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::DELETE);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 0);
    EXPECT_EQ(f.parser_.content_length(), 0);
}

TEST(http_parser, unknown_method_in_request)
{
    fixture f("DELTETE / HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, 'T');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string(""));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 0);
    EXPECT_EQ(f.parser_.content_length(), 0);
}

TEST(http_parser, unknown_url_in_request)
{
    fixture f("DELETE \\ HTTP/1.1\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\\');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::DELETE);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string(""));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 0);
    EXPECT_EQ(f.parser_.content_length(), 0);
}

TEST(http_parser, unknown_version_in_request)
{
    fixture f("DELETE / HTTP/0.0\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '0');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::DELETE);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 0);
    EXPECT_EQ(f.parser_.content_length(), 0);
}

TEST(http_parser, unknown_first_line_end_in_request)
{
    fixture f("DELETE / HTTP/1.1 bla\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, ' ');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::DELETE);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.c_str(), std::string("/"));
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.size(), 0);
    EXPECT_EQ(f.parser_.content_length(), 0);
}

TEST(http_parser, gone_response)
{
    fixture f("HTTP/1.1 410 Gone\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 410);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.c_str(), std::string("Gone"));
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.reason_phrase(), "Gone");
}

TEST(http_parser, not_found_response)
{
    fixture f("HTTP/1.1 404 Not Found\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 404);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.c_str(), std::string("Not Found"));
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.reason_phrase(), "Not Found");
}

TEST(http_parser, custom_response)
{
    fixture f("HTTP/1.1 555 X0Y1Z2\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 555);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.c_str(), std::string("X0Y1Z2"));
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.reason_phrase(), "X0Y1Z2");
}

TEST(http_parser, another_custom_response)
{
    fixture f("HTTP/1.1 555 9X0Y1Z2\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 555);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.c_str(), std::string("9X0Y1Z2"));
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.reason_phrase(), "9X0Y1Z2");
}

TEST(http_parser, unknown_http_version_in_response)
{
    fixture f("HTTP/0.0 200 OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '0');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.c_str(), std::string(""));
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
}

TEST(http_parser, unknown_status_code_in_response)
{
    fixture f("HTTP/1.1 x11 OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, 'x');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.c_str(), std::string(""));
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
}

TEST(http_parser, unknown_reason_phrase_in_response)
{
    fixture f("HTTP/1.1 200 ~OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, '~');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 200);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.c_str(), std::string(""));
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
}

TEST(http_parser, http_error)
{
    fixture f("abcdefghijklmnopqrstuvwxyz");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.httpscan_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.httpscan_.lexer_.last_char_, 'a');
    EXPECT_EQ(f.parser_.httpscan_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.method_, method::UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.httpscan_.url_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.status_code_, 0);
    EXPECT_EQ(f.parser_.httpscan_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.httpscan_.headers_.empty(), true);
}

} // namespace http

} // namespace rest
