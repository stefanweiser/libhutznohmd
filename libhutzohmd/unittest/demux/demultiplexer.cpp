/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "demux/demultiplexer.hpp"
#include "libhutznohmd/mock_request.hpp"

using namespace testing;

namespace hutzn
{

class demultiplexer_test : public Test
{
public:
    void SetUp(void) override
    {
        demultiplexer_ = make_demultiplexer();

        mt_audio_ = demultiplexer_->register_mime_type("audio");
        mt_image_ = demultiplexer_->register_mime_type("image");
        mt_text_ = demultiplexer_->register_mime_type("text");
        mt_video_ = demultiplexer_->register_mime_type("video");

        ms_plain_ = demultiplexer_->register_mime_subtype("plain");
    }

    void TearDown(void) override
    {
        demultiplexer_.reset();
    }

protected:
    request_handler_id id(void)
    {
        static request_handler_id result{"/", http_verb::GET,
                                         mime(mt_text_, ms_plain_),
                                         mime(mt_image_, ms_plain_)};
        return result;
    }

    demux_ptr demultiplexer_;
    mime_type mt_audio_;
    mime_type mt_image_;
    mime_type mt_text_;
    mime_type mt_video_;
    mime_subtype ms_plain_;
};

http_status_code handler_fn(const request&, response&)
{
    return http_status_code::OK;
}

TEST_F(demultiplexer_test, construction)
{
    EXPECT_TRUE(NULL != demultiplexer_.get());
}

TEST_F(demultiplexer_test, connect_disconnect)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());

    handler_ptr handler = demultiplexer_->connect(id(), &handler_fn);

    EXPECT_TRUE(NULL != handler.get());
}

TEST_F(demultiplexer_test, connect_twice)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());

    handler_ptr handler1 = demultiplexer_->connect(id(), &handler_fn);
    handler_ptr handler2 = demultiplexer_->connect(id(), &handler_fn);

    EXPECT_TRUE(NULL != handler1.get());
    EXPECT_EQ(NULL, handler2.get());
}

TEST_F(demultiplexer_test, connect_wrong_path)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());

    request_handler_id test_id = id();
    test_id.path = "/?";
    EXPECT_EQ(NULL, demultiplexer_->connect(test_id, &handler_fn).get());
}

TEST_F(demultiplexer_test, connect_wrong_mime_types)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());

    auto test_id = id();
    test_id.content_type.first = static_cast<mime_type>(100);
    EXPECT_EQ(NULL, demultiplexer_->connect(test_id, &handler_fn).get());

    test_id = id();
    test_id.content_type.second = static_cast<mime_subtype>(100);
    EXPECT_EQ(NULL, demultiplexer_->connect(test_id, &handler_fn).get());

    test_id = id();
    test_id.accept_type.first = static_cast<mime_type>(100);
    EXPECT_EQ(NULL, demultiplexer_->connect(test_id, &handler_fn).get());

    test_id = id();
    test_id.accept_type.second = static_cast<mime_subtype>(100);
    EXPECT_EQ(NULL, demultiplexer_->connect(test_id, &handler_fn).get());
}

TEST_F(demultiplexer_test, determine_request_unknown_path)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());

    auto request = std::make_shared<request_mock>();
    EXPECT_CALL(*request, content_type())
        .Times(1)
        .WillOnce(Return(id().content_type));
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_unknown_method)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());
    handler_ptr handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_TRUE(NULL != handler.get());

    auto request = std::make_shared<request_mock>();
    EXPECT_CALL(*request, content_type())
        .Times(1)
        .WillOnce(Return(id().content_type));
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(http_verb::PUT));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_wildcard_content_type)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());
    handler_ptr handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_TRUE(NULL != handler.get());

    auto request = std::make_shared<request_mock>();
    const auto ct = mime(mime_type::WILDCARD, ms_plain_);
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(Return(ct));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_wildcard_content_subtype)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());
    handler_ptr handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_TRUE(NULL != handler.get());

    auto request = std::make_shared<request_mock>();
    const auto ct = mime(mt_text_, mime_subtype::WILDCARD);
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(Return(ct));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, determine_request_handler_unknown_content_type)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());
    handler_ptr handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_TRUE(NULL != handler.get());

    auto request = std::make_shared<request_mock>();
    const auto ct = mime(mt_video_, ms_plain_);
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type()).Times(1).WillOnce(Return(ct));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, DISABLED_determine_request_handler_unknown_accept_type)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());
    handler_ptr handler = demultiplexer_->connect(id(), &handler_fn);
    ASSERT_TRUE(NULL != handler.get());

    auto request = std::make_shared<request_mock>();
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type())
        .Times(1)
        .WillOnce(Return(id().content_type));
    EXPECT_CALL(*request, accept(_, _))
        .Times(1)
        .WillOnce(Invoke([](void*&, mime&) { return false; }));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, DISABLED_determine_request_handler_success_1)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());
    request_handler_id test_id = id();
    handler_ptr handler = demultiplexer_->connect(test_id, &handler_fn);
    ASSERT_TRUE(NULL != handler.get());

    auto request = std::make_shared<request_mock>();
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type())
        .Times(1)
        .WillOnce(Return(test_id.content_type));
    EXPECT_CALL(*request, accept(_, _))
        .Times(1)
        .WillOnce(Invoke([](void*&, mime& m) {
            m.first = mime_type::WILDCARD;
            m.second = mime_subtype::WILDCARD;
            return true;
        }));

    EXPECT_TRUE(!!demultiplexer_->determine_request_handler(*request));
}

TEST_F(demultiplexer_test, DISABLED_determine_request_handler_success_2)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());
    request_handler_id test_id = id();
    handler_ptr handler = demultiplexer_->connect(test_id, &handler_fn);
    ASSERT_TRUE(NULL != handler.get());

    auto request = std::make_shared<request_mock>();
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type())
        .Times(1)
        .WillOnce(Return(test_id.content_type));
    EXPECT_CALL(*request, accept(_, _))
        .Times(2)
        .WillOnce(Invoke([this](void*&, mime& m) {
            m.first = mt_audio_;
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

TEST_F(demultiplexer_test, DISABLED_determine_request_handler_disabled_handler)
{
    ASSERT_TRUE(NULL != demultiplexer_.get());
    request_handler_id test_id = id();
    handler_ptr handler = demultiplexer_->connect(test_id, &handler_fn);
    ASSERT_TRUE(NULL != handler.get());
    handler->disable();

    auto request = std::make_shared<request_mock>();
    EXPECT_CALL(*request, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request, content_type())
        .Times(1)
        .WillOnce(Return(test_id.content_type));
    EXPECT_CALL(*request, accept(_, _))
        .Times(2)
        .WillOnce(Invoke([this](void*&, mime& m) {
            m.first = mt_image_;
            m.second = ms_plain_;
            return true;
        }))
        .WillOnce(Invoke([](void*&, mime&) { return false; }));

    EXPECT_FALSE(demultiplexer_->determine_request_handler(*request));
    EXPECT_FALSE(handler->is_enabled());

    auto request2 = std::make_shared<request_mock>();
    EXPECT_CALL(*request2, path()).Times(1).WillOnce(Return(id().path.c_str()));
    EXPECT_CALL(*request2, method()).Times(1).WillOnce(Return(id().method));
    EXPECT_CALL(*request2, content_type())
        .Times(1)
        .WillOnce(Return(test_id.content_type));
    EXPECT_CALL(*request2, accept(_, _))
        .Times(1)
        .WillOnce(Invoke([this](void*&, mime& m) {
            m.first = mt_image_;
            m.second = ms_plain_;
            return true;
        }));

    handler->enable();
    EXPECT_TRUE(!!demultiplexer_->determine_request_handler(*request2));
    EXPECT_TRUE(handler->is_enabled());
}

} // namespace hutzn
