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

#include "libhutznohmd/mock_communication.hpp"
#include "request/memory_allocating_request.hpp"

using namespace testing;

namespace hutzn
{

class memory_allocating_request_test : public Test
{
public:
    void SetUp(void) override
    {
        connection_ = std::make_shared<connection_mock>();
        text_type_ = handler_.register_mime_type("text");
        plain_subtype_ = handler_.register_mime_subtype("plain");
    }

    void TearDown(void) override
    {
        ASSERT_TRUE(handler_.unregister_mime_subtype(plain_subtype_));
        ASSERT_TRUE(handler_.unregister_mime_type(text_type_));
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

    void check_request_data(const memory_allocating_request& r)
    {
        EXPECT_EQ(http_verb::GET, r.method());
        EXPECT_STREQ(NULL, r.path());
        EXPECT_EQ(NULL, r.host());
        EXPECT_EQ(NULL, r.query(NULL));
        EXPECT_EQ(NULL, r.fragment());
        EXPECT_EQ(http_version::HTTP_UNKNOWN, r.version());
        EXPECT_EQ(NULL, r.header_value(NULL));
        EXPECT_EQ(false, r.keeps_connection());
        EXPECT_EQ(0, r.date());
        EXPECT_EQ(NULL, r.content());
        EXPECT_EQ(0, r.content_length());
        EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
                  r.content_type());

        void* handle = NULL;
        mime m{mime_type::INVALID, mime_subtype::INVALID};
        EXPECT_EQ(false, r.accept(handle, m));

        EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
        EXPECT_EQ(NULL, r.from());
        EXPECT_EQ(NULL, r.referer());
        EXPECT_EQ(NULL, r.user_agent());
    }

protected:
    connection_mock_ptr connection_;
    mime_handler handler_;
    mime_type text_type_;
    mime_subtype plain_subtype_;
};

TEST_F(memory_allocating_request_test, construction)
{
    memory_allocating_request r{connection_};
    check_request_data(r);
}

TEST_F(memory_allocating_request_test, default_request)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.1\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, request_with_content)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.1\r\nContent-Length: 12\r\n\r\nHello World!");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(0, r.date());

    // fetch_content() will set the content's result.
    EXPECT_EQ(12, r.content_length());
    EXPECT_EQ(NULL, r.content());
    r.fetch_content();
    EXPECT_EQ(12, r.content_length());
    EXPECT_EQ(0, memcmp("Hello World!", r.content(), r.content_length()));

    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, request_with_content_type)
{
    memory_allocating_request r{connection_};
    setup_receive(
        "GET / HTTP/1.1\r\nContent-Length: 12\r\nContent-Type: "
        "text/plain\r\n\r\nHello World!");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(0, r.date());

    // fetch_content() will set the content's result.
    EXPECT_EQ(12, r.content_length());
    EXPECT_EQ(mime(text_type_, plain_subtype_), r.content_type());
    EXPECT_EQ(NULL, r.content());
    r.fetch_content();
    EXPECT_EQ(12, r.content_length());
    EXPECT_EQ(mime(text_type_, plain_subtype_), r.content_type());
    EXPECT_EQ(0, memcmp("Hello World!", r.content(), r.content_length()));

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, http_1_0_request_without_keep_alive)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.0\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_0, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(false, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, http_1_0_request_with_non_keep_alive)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.0\r\nConnection: close\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_0, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(false, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, http_1_0_request_with_keep_alive)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.0\r\nConnection: keep-alive\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_0, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, request_with_date)
{
    memory_allocating_request r{connection_};
    setup_receive(
        "GET / HTTP/1.1\r\nDate: Sun, 06 Nov 1994 08:49:37 GMT\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(784111777, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, request_with_from)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.1\r\nFrom: info@example.com\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ("info@example.com", r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, request_with_referer)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.1\r\nReferer: http://www.google.com/\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ("http://www.google.com/", r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, request_with_user_agent)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.1\r\nUser-Agent: libhutznohmd/0.0.1\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ(NULL, r.header_value(NULL));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ("libhutznohmd/0.0.1", r.user_agent());
}

TEST_F(memory_allocating_request_test, custom_header)
{
    memory_allocating_request r{connection_};
    setup_receive("GET / HTTP/1.1\r\na:b\r\n\r\n");
    ASSERT_TRUE(r.parse(handler_));

    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_STREQ("/", r.path());
    EXPECT_STREQ(NULL, r.host());
    EXPECT_STREQ(NULL, r.query(NULL));
    EXPECT_STREQ(NULL, r.fragment());
    EXPECT_EQ(http_version::HTTP_1_1, r.version());
    EXPECT_STREQ("b", r.header_value("a"));
    EXPECT_EQ(true, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(NULL, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = NULL;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_STREQ(NULL, r.from());
    EXPECT_STREQ(NULL, r.referer());
    EXPECT_STREQ(NULL, r.user_agent());
}

TEST_F(memory_allocating_request_test, parsing_method_failed_because_no_data)
{
    memory_allocating_request r{connection_};
    setup_receive(" ");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_method_failed_because_no_whitespace_found)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_method_failed_because_not_a_method)
{
    memory_allocating_request r{connection_};
    setup_receive("ARGHH ");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_method_failed_because_method_token_too_long)
{
    memory_allocating_request r{connection_};
    setup_receive("PUTT ");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_method_failed_because_method_token_much_too_long)
{
    memory_allocating_request r{connection_};
    setup_receive("DELETEE ");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test, parsing_uri_failed_because_no_data)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT ");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_uri_failed_because_no_whitespace_found)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT /");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test, parsing_version_failed_because_no_data)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT / ");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_version_failed_because_no_newline_found)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT / HTTP/1.1");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_version_failed_because_not_a_version)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT / HTTP/x.x\n");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_version_failed_because_version_too_long)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT / HTTP/22\n");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_version_failed_because_version_much_too_long)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT / HTTP/1.11\n");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test, parsing_header_failed_because_no_data)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT / HTTP/1.1\na");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

TEST_F(memory_allocating_request_test,
       parsing_header_failed_because_no_newline_found)
{
    memory_allocating_request r{connection_};
    setup_receive("PUT / HTTP/2\na:b");
    EXPECT_FALSE(r.parse(handler_));
    check_request_data(r);
}

} // namespace hutzn
