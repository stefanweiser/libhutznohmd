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
#include <demux/demultiplexer_accept_map.hpp>

using namespace testing;

namespace hutzn
{

class demultiplexer_accept_map_test : public ::testing::Test
{
protected:
    static std::function<bool(void*&, mime&)> make_accept_fn(const mime& type)
    {
        return [type](void*&, mime& t) {
            t = type;
            return true;
        };
    }

    static request_handler_callback make_request_handler(
        const http_status_code& code)
    {
        return [code](const request_interface&,
                      response_interface&) { return code; };
    }

    const mime none_{mime_type::NONE, mime_subtype::NONE};
    const mime text_plain_{mime_type::TEXT, mime_subtype::PLAIN};
};

TEST_F(demultiplexer_accept_map_test, size_of_empty)
{
    demultiplexer_accept_map map;
    EXPECT_EQ(map.size(), 0);
}

TEST_F(demultiplexer_accept_map_test, size_of_one_inserted)
{
    demultiplexer_accept_map map;
    map.insert(none_, request_handler_callback());
    EXPECT_EQ(map.size(), 1);
}

TEST_F(demultiplexer_accept_map_test, size_of_twice_inserted_the_same)
{
    demultiplexer_accept_map map;
    map.insert(none_, request_handler_callback());
    map.insert(none_, request_handler_callback());
    EXPECT_EQ(map.size(), 1);
}

TEST_F(demultiplexer_accept_map_test, size_of_inserted_two_different)
{
    demultiplexer_accept_map map;
    map.insert(none_, request_handler_callback());
    map.insert(text_plain_, request_handler_callback());
    EXPECT_EQ(map.size(), 2);
}

TEST_F(demultiplexer_accept_map_test, twice_inserted_the_same)
{
    demultiplexer_accept_map map;
    EXPECT_TRUE(map.insert(none_, request_handler_callback()));
    EXPECT_FALSE(map.insert(none_, request_handler_callback()));
}

TEST_F(demultiplexer_accept_map_test, inserted_two_different)
{
    demultiplexer_accept_map map;
    EXPECT_TRUE(map.insert(none_, request_handler_callback()));
    EXPECT_TRUE(map.insert(text_plain_, request_handler_callback()));
}

TEST_F(demultiplexer_accept_map_test, inserting_wildcard_is_failing)
{
    demultiplexer_accept_map map;

    mime type = mime(mime_type::WILDCARD, mime_subtype::PLAIN);
    EXPECT_FALSE(map.insert(type, request_handler_callback()));

    type = mime(mime_type::TEXT, mime_subtype::WILDCARD);
    EXPECT_FALSE(map.insert(type, request_handler_callback()));

    type = mime(mime_type::WILDCARD, mime_subtype::WILDCARD);
    EXPECT_FALSE(map.insert(type, request_handler_callback()));
}

TEST_F(demultiplexer_accept_map_test, erase_nonexistent)
{
    demultiplexer_accept_map map;
    EXPECT_FALSE(map.erase(none_));
}

TEST_F(demultiplexer_accept_map_test, erase_existent_when_one_is_inserted)
{
    demultiplexer_accept_map map;
    map.insert(none_, request_handler_callback());
    EXPECT_TRUE(map.erase(none_));
    EXPECT_EQ(map.size(), 0);
}

TEST_F(demultiplexer_accept_map_test, erase_existent_when_two_are_inserted)
{
    demultiplexer_accept_map map;
    map.insert(none_, request_handler_callback());
    map.insert(text_plain_, request_handler_callback());
    EXPECT_TRUE(map.erase(none_));
    EXPECT_EQ(map.size(), 1);
}

TEST_F(demultiplexer_accept_map_test, erase_two_existent_when_two_are_inserted)
{
    demultiplexer_accept_map map;
    map.insert(none_, request_handler_callback());
    map.insert(text_plain_, request_handler_callback());
    EXPECT_TRUE(map.erase(none_));
    EXPECT_TRUE(map.erase(text_plain_));
    EXPECT_EQ(map.size(), 0);
}

TEST_F(demultiplexer_accept_map_test, find_in_empty_vector)
{
    demultiplexer_accept_map map;
    EXPECT_FALSE(map.find_ordered(none_));
}

TEST_F(demultiplexer_accept_map_test, find_none_in_vector)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));

    request_handler_callback none_fn = map.find_ordered(none_);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_fn(request, response), http_status_code::OK);
}

TEST_F(demultiplexer_accept_map_test, find_none_in_vector_second_time)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(text_plain_, make_request_handler(http_status_code::FOUND));
    map.insert(none_, make_request_handler(http_status_code::OK));

    request_handler_callback none_fn = map.find_ordered(none_);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_fn(request, response), http_status_code::OK);
}

TEST_F(demultiplexer_accept_map_test, find_wildcard_type_in_vector)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));

    const mime wildcard = mime(mime_type::WILDCARD, mime_subtype::NONE);
    request_handler_callback none_fn = map.find_ordered(wildcard);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_fn(request, response), http_status_code::OK);
}

TEST_F(demultiplexer_accept_map_test, find_wildcard_subtype_in_vector)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));

    const mime wildcard = mime(mime_type::NONE, mime_subtype::WILDCARD);
    request_handler_callback none_fn = map.find_ordered(wildcard);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_fn(request, response), http_status_code::OK);
}

TEST_F(demultiplexer_accept_map_test, find_wildcard_in_vector)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));

    const mime wildcard = mime(mime_type::WILDCARD, mime_subtype::WILDCARD);
    request_handler_callback none_fn = map.find_ordered(wildcard);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_fn(request, response), http_status_code::OK);
}

TEST_F(demultiplexer_accept_map_test, find_nothing_accepted_in_empty_map)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    EXPECT_CALL(request, accept(_, _)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(map.find(request));
}

TEST_F(demultiplexer_accept_map_test, find_none_accepted_in_empty_map)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    EXPECT_CALL(request, accept(_, _))
        .Times(2)
        .WillOnce(Invoke(make_accept_fn(none_)))
        .WillOnce(Return(false));

    EXPECT_FALSE(map.find(request));
}

TEST_F(demultiplexer_accept_map_test,
       find_none_and_text_plain_accepted_in_empty_map)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    EXPECT_CALL(request, accept(_, _))
        .Times(3)
        .WillOnce(Invoke(make_accept_fn(none_)))
        .WillOnce(Invoke(make_accept_fn(text_plain_)))
        .WillOnce(Return(false));

    EXPECT_FALSE(map.find(request));
}

TEST_F(demultiplexer_accept_map_test, find_none_accepted_while_none_is_inserted)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));
    EXPECT_CALL(request, accept(_, _)).Times(1).WillOnce(
        Invoke(make_accept_fn(none_)));

    request_handler_callback none_fn = map.find(request);
    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_fn(request, response), http_status_code::OK);
}

TEST_F(demultiplexer_accept_map_test,
       find_none_accepted_while_both_are_inserted)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));
    map.insert(text_plain_, make_request_handler(http_status_code::FOUND));
    EXPECT_CALL(request, accept(_, _)).Times(1).WillOnce(
        Invoke(make_accept_fn(none_)));

    request_handler_callback none_fn = map.find(request);
    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_fn(request, response), http_status_code::OK);
}

TEST_F(demultiplexer_accept_map_test,
       find_text_plain_accepted_while_both_are_inserted)
{
    demultiplexer_accept_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));
    map.insert(text_plain_, make_request_handler(http_status_code::FOUND));
    EXPECT_CALL(request, accept(_, _)).Times(1).WillOnce(
        Invoke(make_accept_fn(text_plain_)));

    request_handler_callback text_plain_fn = map.find(request);
    ASSERT_TRUE(!!text_plain_fn);
    EXPECT_EQ(text_plain_fn(request, response), http_status_code::FOUND);
}

} // namespace hutzn
