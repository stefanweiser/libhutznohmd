/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

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

#include <libhutznohmd/mock_sockets.hpp>

#include <request/request.hpp>

using namespace testing;

namespace hutzn
{

class request_test : public ::testing::Test
{
public:
    void SetUp(void) override
    {
        connection_ = std::make_shared<connection_interface_mock>();
    }

    void TearDown(void) override
    {
        connection_.reset();
    }

    void setup_receive(const std::string& chunk)
    {
        EXPECT_CALL(*connection_, receive(_, _))
            .Times(AtLeast(1))
            .WillOnce(Invoke([chunk](buffer& b, const size_t& m) {
                EXPECT_LE(chunk.size(), m);
                b.insert(b.begin(), chunk.begin(), chunk.end());
                return true;
            }))
            .WillRepeatedly(Return(false));
    }

    void check_request_data(const request& r)
    {
        EXPECT_EQ(http_verb::GET, r.method());
        EXPECT_STREQ(nullptr, r.path());
        EXPECT_EQ(nullptr, r.host());
        EXPECT_EQ(nullptr, r.query(nullptr));
        EXPECT_EQ(nullptr, r.fragment());
        EXPECT_EQ(http_version::HTTP_UNKNOWN, r.version());
        EXPECT_EQ(nullptr, r.header_value(nullptr));
        EXPECT_EQ(false, r.keeps_connection());
        EXPECT_EQ(0, r.date());
        EXPECT_EQ(nullptr, r.content());
        EXPECT_EQ(0, r.content_length());
        EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
                  r.content_type());

        void* handle = nullptr;
        mime m{mime_type::INVALID, mime_subtype::INVALID};
        EXPECT_EQ(false, r.accept(handle, m));

        EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
        EXPECT_EQ(nullptr, r.from());
        EXPECT_EQ(nullptr, r.referer());
        EXPECT_EQ(nullptr, r.user_agent());
    }

protected:
    connection_mock_pointer connection_;
};

TEST_F(request_test, construction)
{
    request r{connection_};
    check_request_data(r);
}

TEST_F(request_test, default_request)
{
    request r{connection_};
    setup_receive("GET / HTTP/1.1\r\n\r\n");
    ASSERT_TRUE(r.parse());

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("", r.path());
    EXPECT_STREQ(nullptr, r.host());
    EXPECT_STREQ(nullptr, r.query(nullptr));
    EXPECT_STREQ(nullptr, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(nullptr, r.header_value(nullptr));
    EXPECT_EQ(false, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(nullptr, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = nullptr;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(nullptr, r.from());
    EXPECT_STREQ(nullptr, r.referer());
    EXPECT_STREQ(nullptr, r.user_agent());
}

TEST_F(request_test, request_with_content)
{
    request r{connection_};
    setup_receive("GET / HTTP/1.1\r\nContent-Length: 12\r\n\r\nHello World!");
    ASSERT_TRUE(r.parse());

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("", r.path());
    EXPECT_STREQ(nullptr, r.host());
    EXPECT_STREQ(nullptr, r.query(nullptr));
    EXPECT_STREQ(nullptr, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(nullptr, r.header_value(nullptr));
    EXPECT_EQ(false, r.keeps_connection());
    EXPECT_EQ(0, r.date());

    // fetch_content() will set the content's result.
    EXPECT_EQ(12, r.content_length());
    EXPECT_EQ(nullptr, r.content());
    r.fetch_content();
    EXPECT_EQ(12, r.content_length());
    EXPECT_EQ(0, memcmp("Hello World!", r.content(), r.content_length()));

    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = nullptr;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(nullptr, r.from());
    EXPECT_STREQ(nullptr, r.referer());
    EXPECT_STREQ(nullptr, r.user_agent());
}

TEST_F(request_test, request_with_timestamp)
{
    request r{connection_};
    setup_receive(
        "GET / HTTP/1.1\r\nDate: Sun, 06 Nov 1994 08:49:37 GMT\r\n\r\n");
    ASSERT_TRUE(r.parse());

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("", r.path());
    EXPECT_STREQ(nullptr, r.host());
    EXPECT_STREQ(nullptr, r.query(nullptr));
    EXPECT_STREQ(nullptr, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(nullptr, r.header_value(nullptr));
    EXPECT_EQ(false, r.keeps_connection());
    EXPECT_EQ(784111777, r.date());
    EXPECT_EQ(nullptr, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = nullptr;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(nullptr, r.from());
    EXPECT_STREQ(nullptr, r.referer());
    EXPECT_STREQ(nullptr, r.user_agent());
}

TEST_F(request_test, request_with_user_agent)
{
    request r{connection_};
    setup_receive("GET / HTTP/1.1\r\nUser-Agent: libhutznohmd/0.0.1\r\n\r\n");
    ASSERT_TRUE(r.parse());

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("", r.path());
    EXPECT_STREQ(nullptr, r.host());
    EXPECT_STREQ(nullptr, r.query(nullptr));
    EXPECT_STREQ(nullptr, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(nullptr, r.header_value(nullptr));
    EXPECT_EQ(false, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(nullptr, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = nullptr;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(nullptr, r.from());
    EXPECT_STREQ(nullptr, r.referer());
    EXPECT_STREQ("libhutznohmd/0.0.1", r.user_agent());
}

TEST_F(request_test, custom_header)
{
    request r{connection_};
    setup_receive("GET / HTTP/1.1\r\na:b\r\n\r\n");
    ASSERT_TRUE(r.parse());

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("", r.path());
    EXPECT_STREQ(nullptr, r.host());
    EXPECT_STREQ(nullptr, r.query(nullptr));
    EXPECT_STREQ(nullptr, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ("b", r.header_value("a"));
    EXPECT_EQ(false, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(nullptr, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = nullptr;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(nullptr, r.from());
    EXPECT_STREQ(nullptr, r.referer());
    EXPECT_STREQ(nullptr, r.user_agent());
}

TEST_F(request_test, parsing_method_failed_because_no_data)
{
    request r{connection_};
    setup_receive(" ");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_method_failed_because_no_whitespace_found)
{
    request r{connection_};
    setup_receive("PUT");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_method_failed_because_not_a_method)
{
    request r{connection_};
    setup_receive("ARGHH ");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_method_failed_because_method_token_too_long)
{
    request r{connection_};
    setup_receive("PUTT ");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_method_failed_because_method_token_much_too_long)
{
    request r{connection_};
    setup_receive("DELETEE ");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_uri_failed_because_no_data)
{
    request r{connection_};
    setup_receive("PUT ");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_uri_failed_because_no_whitespace_found)
{
    request r{connection_};
    setup_receive("PUT /");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_version_failed_because_no_data)
{
    request r{connection_};
    setup_receive("PUT / ");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_version_failed_because_no_newline_found)
{
    request r{connection_};
    setup_receive("PUT / HTTP/1.1");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_version_failed_because_not_a_version)
{
    request r{connection_};
    setup_receive("PUT / HTTP/x.x\n");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_version_failed_because_version_too_long)
{
    request r{connection_};
    setup_receive("PUT / HTTP/22\n");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_version_failed_because_version_much_too_long)
{
    request r{connection_};
    setup_receive("PUT / HTTP/1.11\n");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_header_failed_because_no_data)
{
    request r{connection_};
    setup_receive("PUT / HTTP/1.1\na");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

TEST_F(request_test, parsing_header_failed_because_no_newline_found)
{
    request r{connection_};
    setup_receive("PUT / HTTP/2\na:b");
    EXPECT_FALSE(r.parse());
    check_request_data(r);
}

} // namespace hutzn
