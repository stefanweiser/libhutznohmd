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

#include <libhutznohmd/mock_request.hpp>
#include <demux/demultiplexer.hpp>

using namespace testing;

namespace hutzn
{

class demultiplexer_test : public ::testing::Test
{
public:
    void SetUp(void) override
    {
        demultiplexer_ = make_demultiplexer();
    }

    void TearDown(void) override
    {
        demultiplexer_.reset();
    }

protected:
    request_handler_id id(void)
    {
        static request_handler_id result{
            "/", http_verb::GET, mime(mime_type::TEXT, mime_subtype::PLAIN),
            mime(mime_type::IMAGE, mime_subtype::PLAIN)};
        return result;
    }

    demux_pointer demultiplexer_;
};

http_status_code handler_fn(const request_interface&, response_interface&)
{
    return http_status_code::OK;
}

TEST_F(demultiplexer_test, construction)
{
    EXPECT_NE(demultiplexer_.get(), nullptr);
}

TEST_F(demultiplexer_test, connect_disconnect)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);

    handler_pointer handler = demultiplexer_->connect(id(), &handler_fn);

    EXPECT_NE(handler.get(), nullptr);
}

TEST_F(demultiplexer_test, connect_twice)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);

    handler_pointer handler1 = demultiplexer_->connect(id(), &handler_fn);
    handler_pointer handler2 = demultiplexer_->connect(id(), &handler_fn);

    EXPECT_NE(handler1.get(), nullptr);
    EXPECT_EQ(handler2.get(), nullptr);
}

TEST_F(demultiplexer_test, connect_wrong_path)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);

    request_handler_id test_id = id();
    test_id.path = "/?";
    EXPECT_EQ(demultiplexer_->connect(test_id, &handler_fn).get(), nullptr);
}

TEST_F(demultiplexer_test, connect_wrong_mime_types)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);

    auto test_id = id();
    test_id.content_type.first = static_cast<mime_type>(100);
    EXPECT_EQ(demultiplexer_->connect(test_id, &handler_fn).get(), nullptr);

    test_id = id();
    test_id.content_type.second = static_cast<mime_subtype>(100);
    EXPECT_EQ(demultiplexer_->connect(test_id, &handler_fn).get(), nullptr);

    test_id = id();
    test_id.accept_type.first = static_cast<mime_type>(100);
    EXPECT_EQ(demultiplexer_->connect(test_id, &handler_fn).get(), nullptr);

    test_id = id();
    test_id.accept_type.second = static_cast<mime_subtype>(100);
    EXPECT_EQ(demultiplexer_->connect(test_id, &handler_fn).get(), nullptr);
}

TEST_F(demultiplexer_test, determine_request_unknown_path)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);

    auto request = std::make_shared<request_interface_mock>();
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(
        Return(id().content_type));
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_unknown_method)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);
    handler_pointer handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_NE(handler.get(), nullptr);

    auto request = std::make_shared<request_interface_mock>();
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(
        Return(id().content_type));
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(http_verb::PUT));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_wildcard_content_type)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);
    handler_pointer handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_NE(handler.get(), nullptr);

    auto request = std::make_shared<request_interface_mock>();
    const auto ct = mime(mime_type::WILDCARD, mime_subtype::PLAIN);
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(Return(ct));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_wildcard_content_subtype)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);
    handler_pointer handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_NE(handler.get(), nullptr);

    auto request = std::make_shared<request_interface_mock>();
    const auto ct = mime(mime_type::TEXT, mime_subtype::WILDCARD);
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(Return(ct));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_unknown_content_type)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);
    handler_pointer handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_NE(handler.get(), nullptr);

    auto request = std::make_shared<request_interface_mock>();
    const auto ct = mime(mime_type::VIDEO, mime_subtype::PLAIN);
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(Return(ct));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_unknown_accept_type)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);
    handler_pointer handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_NE(handler.get(), nullptr);

    auto request = std::make_shared<request_interface_mock>();
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(
        Return(id().content_type));
    EXPECT_CALL(*request, accept(_, _)).Times(1).WillOnce(
        Invoke([](void*&, mime&) { return false; }));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_success_1)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);
    request_handler_id test_id = id();
    handler_pointer handler = demultiplexer_->connect(test_id, &handler_fn);
    ASSERT_NE(handler.get(), nullptr);

    auto request = std::make_shared<request_interface_mock>();
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(
        Return(test_id.content_type));
    EXPECT_CALL(*request, accept(_, _)).Times(1).WillOnce(
        Invoke([](void*&, mime& m) {
            m.first = mime_type::WILDCARD;
            m.second = mime_subtype::WILDCARD;
            return true;
        }));

    EXPECT_TRUE(!!demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_success_2)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);
    request_handler_id test_id = id();
    handler_pointer handler = demultiplexer_->connect(test_id, &handler_fn);
    ASSERT_NE(handler.get(), nullptr);

    auto request = std::make_shared<request_interface_mock>();
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(
        Return(test_id.content_type));
    EXPECT_CALL(*request, accept(_, _))
        .Times(2)
        .WillOnce(Invoke([](void*&, mime& m) {
            m.first = mime_type::AUDIO;
            m.second = mime_subtype::WILDCARD;
            return true;
        }))
        .WillOnce(Invoke([](void*&, mime& m) {
            m.first = mime_type::WILDCARD;
            m.second = mime_subtype::WILDCARD;
            return true;
        }));

    EXPECT_TRUE(!!demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_disabled_handler)
{
    ASSERT_NE(demultiplexer_.get(), nullptr);
    request_handler_id test_id = id();
    handler_pointer handler = demultiplexer_->connect(test_id, &handler_fn);
    ASSERT_NE(handler.get(), nullptr);
    handler->disable();

    auto request = std::make_shared<request_interface_mock>();
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(
        Return(test_id.content_type));
    EXPECT_CALL(*request, accept(_, _))
        .Times(2)
        .WillOnce(Invoke([](void*&, mime& m) {
            m.first = mime_type::IMAGE;
            m.second = mime_subtype::PLAIN;
            return true;
        }))
        .WillOnce(Invoke([](void*&, mime&) { return false; }));

    EXPECT_TRUE(!demultiplexer_->determine_request_handler(*request));
    EXPECT_FALSE(handler->is_enabled());

    auto request2 = std::make_shared<request_interface_mock>();
    EXPECT_CALL(*request2, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request2, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request2, content_type()).Times(1).WillOnce(
        Return(test_id.content_type));
    EXPECT_CALL(*request2, accept(_, _)).Times(1).WillOnce(
        Invoke([](void*&, mime& m) {
            m.first = mime_type::IMAGE;
            m.second = mime_subtype::PLAIN;
            return true;
        }));

    handler->enable();
    EXPECT_TRUE(!!demultiplexer_->determine_request_handler(*request2));
    EXPECT_TRUE(handler->is_enabled());
}

} // namespace hutzn
