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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/parser/responseparser.hpp>

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
        return static_cast<uint8_t>(p->first[p->second++]);
    }
    return -1;
}

int32_t peek_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second]);
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string & request);
    ~fixture();

    string_index_pair str_;
    response_parser parser_;
};

fixture::fixture(const std::string & request)
    : str_(std::make_pair(request, 0))
    , parser_(anonymous_int_function(&get_char, &str_),
              anonymous_int_function(&peek_char, &str_))
{}

fixture::~fixture()
{
    EXPECT_EQ(parser_.valid(), (parser_state::SUCCEEDED == parser_.state_));
    EXPECT_EQ(parser_.version(), parser_.version_);
    EXPECT_EQ(parser_.status_code(), parser_.status_code_);
    EXPECT_EQ(parser_.reason_phrase(), parser_.reason_phrase_.c_str());
}

} // namespace

TEST(response_parser, construction_destruction)
{
    fixture f("");
    EXPECT_EQ(f.parser_.state_, parser_state::UNFINISHED);
    EXPECT_EQ(f.parser_.lexer_.last_char_, 0);
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.status_code_, 0);
    EXPECT_EQ(f.parser_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.headers_.empty(), true);
}

TEST(response_parser, gone_response)
{
    fixture f("HTTP/1.1 410 Gone\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code_, 410);
    EXPECT_EQ(f.parser_.reason_phrase_.c_str(), std::string("Gone"));
    EXPECT_EQ(f.parser_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.reason_phrase(), "Gone");
}

TEST(response_parser, not_found_response)
{
    fixture f("HTTP/1.1 404 Not Found\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code_, 404);
    EXPECT_EQ(f.parser_.reason_phrase_.c_str(), std::string("Not Found"));
    EXPECT_EQ(f.parser_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.reason_phrase(), "Not Found");
}

TEST(response_parser, custom_response)
{
    fixture f("HTTP/1.1 555 X0Y1Z2\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code_, 555);
    EXPECT_EQ(f.parser_.reason_phrase_.c_str(), std::string("X0Y1Z2"));
    EXPECT_EQ(f.parser_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.reason_phrase(), "X0Y1Z2");
}

TEST(response_parser, another_custom_response)
{
    fixture f("HTTP/1.1 555 9X0Y1Z2\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::SUCCEEDED);
    EXPECT_EQ(f.parser_.lexer_.last_char_, '\n');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code_, 555);
    EXPECT_EQ(f.parser_.reason_phrase_.c_str(), std::string("9X0Y1Z2"));
    EXPECT_EQ(f.parser_.headers_.empty(), true);
    EXPECT_EQ(f.parser_.reason_phrase(), "9X0Y1Z2");
}

TEST(response_parser, unknown_http_version_in_response)
{
    fixture f("HTTP/0.0 200 OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.lexer_.last_char_, '0');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.status_code_, 0);
    EXPECT_EQ(f.parser_.reason_phrase_.c_str(), std::string(""));
    EXPECT_EQ(f.parser_.headers_.empty(), true);
}

TEST(response_parser, unknown_status_code_in_response)
{
    fixture f("HTTP/1.1 x11 OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.lexer_.last_char_, 'x');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code_, 0);
    EXPECT_EQ(f.parser_.reason_phrase_.c_str(), std::string(""));
    EXPECT_EQ(f.parser_.headers_.empty(), true);
}

TEST(response_parser, unknown_reason_phrase_in_response)
{
    fixture f("HTTP/1.1 200 ~OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.lexer_.last_char_, '~');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code_, 200);
    EXPECT_EQ(f.parser_.reason_phrase_.c_str(), std::string(""));
    EXPECT_EQ(f.parser_.headers_.empty(), true);
}

TEST(response_parser, unknown_date_in_request)
{
    fixture f("HTTP/1.1 200 OK\r\nABC:\r\nDate:\n a\r\n\r\n");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.lexer_.last_char_, '\xFF');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code_, 200);
    EXPECT_EQ(f.parser_.reason_phrase_.c_str(), std::string("OK"));
    EXPECT_EQ(f.parser_.headers_.size(), 1);
    EXPECT_EQ(f.parser_.content_length(), 0);
}

TEST(response_parser, http_error)
{
    fixture f("abcdefghijklmnopqrstuvwxyz");
    f.parser_.parse();
    EXPECT_EQ(f.parser_.state_, parser_state::ERROR);
    EXPECT_EQ(f.parser_.lexer_.last_char_, 'a');
    EXPECT_EQ(f.parser_.header_key_.empty(), true);
    EXPECT_EQ(f.parser_.header_value_.empty(), true);
    EXPECT_EQ(f.parser_.version_, version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.status_code_, 0);
    EXPECT_EQ(f.parser_.reason_phrase_.empty(), true);
    EXPECT_EQ(f.parser_.headers_.empty(), true);
}

} // namespace http

} // namespace rest
