/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The libhutznohmd project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The libhutznohmd project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the libhutznohmd project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <http/parser/response_parser.hpp>

using namespace testing;

namespace rest
{

namespace http
{

namespace
{

using string_index_pair = std::pair<std::string, size_t>;

int32_t get_char(void* handle)
{
    string_index_pair* p = static_cast<string_index_pair*>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second++]);
    }
    return -1;
}

int32_t peek_char(void* handle)
{
    string_index_pair* p = static_cast<string_index_pair*>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second]);
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string& request);

    string_index_pair str_;
    response_parser parser_;
};

fixture::fixture(const std::string& request)
    : str_(std::make_pair(request, 0))
    , parser_(anonymous_int_function(&get_char, &str_),
              anonymous_int_function(&peek_char, &str_))
{
}

} // namespace

TEST(response_parser, construction_destruction)
{
    fixture f("");
    EXPECT_FALSE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, 0);
    EXPECT_EQ(f.parser_.version(), version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.status_code(), 0);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string());
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

TEST(response_parser, gone_response)
{
    fixture f("HTTP/1.1 410 Gone\r\n\r\n");
    f.parser_.parse();
    EXPECT_TRUE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, f.str_.first.size());
    EXPECT_EQ(f.parser_.version(), version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code(), 410);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string("Gone"));
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

TEST(response_parser, not_found_response)
{
    fixture f("HTTP/1.1 404 Not Found\r\n\r\n");
    f.parser_.parse();
    EXPECT_TRUE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, f.str_.first.size());
    EXPECT_EQ(f.parser_.version(), version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code(), 404);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string("Not Found"));
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

TEST(response_parser, custom_response)
{
    fixture f("HTTP/1.1 555 X0Y1Z2\r\n\r\n");
    f.parser_.parse();
    EXPECT_TRUE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, f.str_.first.size());
    EXPECT_EQ(f.parser_.version(), version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code(), 555);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string("X0Y1Z2"));
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

TEST(response_parser, another_custom_response)
{
    fixture f("HTTP/1.1 555 9X0Y1Z2\r\n\r\n");
    f.parser_.parse();
    EXPECT_TRUE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, f.str_.first.size());
    EXPECT_EQ(f.parser_.version(), version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code(), 555);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string("9X0Y1Z2"));
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

TEST(response_parser, unknown_http_version_in_response)
{
    fixture f("HTTP/0.0 200 OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_FALSE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, 6);
    EXPECT_EQ(f.parser_.version(), version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.status_code(), 0);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string());
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

TEST(response_parser, unknown_status_code_in_response)
{
    fixture f("HTTP/1.1 x11 OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_FALSE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, 10);
    EXPECT_EQ(f.parser_.version(), version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code(), 0);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string());
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

TEST(response_parser, unknown_reason_phrase_in_response)
{
    fixture f("HTTP/1.1 200 ~OK\r\n\r\n");
    f.parser_.parse();
    EXPECT_FALSE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, 14);
    EXPECT_EQ(f.parser_.version(), version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code(), 200);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string());
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

TEST(response_parser, unknown_date_in_request)
{
    fixture f("HTTP/1.1 200 OK\r\nABC:\r\nDate:\n a\r\n\r\n");
    f.parser_.parse();
    EXPECT_FALSE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, f.str_.first.size());
    EXPECT_EQ(f.parser_.version(), version::HTTP_1_1);
    EXPECT_EQ(f.parser_.status_code(), 200);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string("OK"));
    EXPECT_EQ(f.parser_.headers().size(), 1);
    EXPECT_EQ(f.parser_.content_length(), 0);
}

TEST(response_parser, http_error)
{
    fixture f("abcdefghijklmnopqrstuvwxyz");
    f.parser_.parse();
    EXPECT_FALSE(f.parser_.valid());
    EXPECT_EQ(f.str_.second, 1);
    EXPECT_EQ(f.parser_.version(), version::HTTP_UNKNOWN);
    EXPECT_EQ(f.parser_.status_code(), 0);
    EXPECT_EQ(f.parser_.reason_phrase(), std::string());
    EXPECT_EQ(f.parser_.headers().empty(), true);
}

} // namespace http

} // namespace rest
