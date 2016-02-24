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

#include "demux/demultiplexer_ordered_mime_map.hpp"
#include "libhutznohmd/mock_request.hpp"

using namespace testing;

namespace hutzn
{

class demultiplexer_ordered_mime_map_test : public Test
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
        return [code](const request_interface&, response_interface&) {
            return code;
        };
    }

    const mime none_{mime_type::NONE, mime_subtype::NONE};
    const mime another_{static_cast<mime_type>(100),
                        static_cast<mime_subtype>(100)};
};

TEST_F(demultiplexer_ordered_mime_map_test, size_of_empty)
{
    demultiplexer_ordered_mime_map map;
    EXPECT_EQ(0, map.size());
}

TEST_F(demultiplexer_ordered_mime_map_test, size_of_one_inserted)
{
    demultiplexer_ordered_mime_map map;
    map.insert(none_, request_handler_callback());
    EXPECT_EQ(1, map.size());
}

TEST_F(demultiplexer_ordered_mime_map_test, size_of_twice_inserted_the_same)
{
    demultiplexer_ordered_mime_map map;
    map.insert(none_, request_handler_callback());
    map.insert(none_, request_handler_callback());
    EXPECT_EQ(1, map.size());
}

TEST_F(demultiplexer_ordered_mime_map_test, size_of_inserted_two_different)
{
    demultiplexer_ordered_mime_map map;
    map.insert(none_, request_handler_callback());
    map.insert(another_, request_handler_callback());
    EXPECT_EQ(2, map.size());
}

TEST_F(demultiplexer_ordered_mime_map_test, twice_inserted_the_same)
{
    demultiplexer_ordered_mime_map map;
    EXPECT_TRUE(map.insert(none_, request_handler_callback()));
    EXPECT_FALSE(map.insert(none_, request_handler_callback()));
}

TEST_F(demultiplexer_ordered_mime_map_test, inserted_two_different)
{
    demultiplexer_ordered_mime_map map;
    EXPECT_TRUE(map.insert(none_, request_handler_callback()));
    EXPECT_TRUE(map.insert(another_, request_handler_callback()));
}

TEST_F(demultiplexer_ordered_mime_map_test, inserting_wildcard_is_failing)
{
    demultiplexer_ordered_mime_map map;

    mime type = mime(mime_type::WILDCARD, static_cast<mime_subtype>(100));
    EXPECT_FALSE(map.insert(type, request_handler_callback()));

    type = mime(static_cast<mime_type>(100), mime_subtype::WILDCARD);
    EXPECT_FALSE(map.insert(type, request_handler_callback()));

    type = mime(mime_type::WILDCARD, mime_subtype::WILDCARD);
    EXPECT_FALSE(map.insert(type, request_handler_callback()));
}

TEST_F(demultiplexer_ordered_mime_map_test, erase_nonexistent)
{
    demultiplexer_ordered_mime_map map;
    EXPECT_FALSE(map.erase(none_));
}

TEST_F(demultiplexer_ordered_mime_map_test, erase_existent_when_one_is_inserted)
{
    demultiplexer_ordered_mime_map map;
    map.insert(none_, request_handler_callback());
    EXPECT_TRUE(map.erase(none_));
    EXPECT_EQ(0, map.size());
}

TEST_F(demultiplexer_ordered_mime_map_test,
       erase_existent_when_two_are_inserted)
{
    demultiplexer_ordered_mime_map map;
    map.insert(none_, request_handler_callback());
    map.insert(another_, request_handler_callback());
    EXPECT_TRUE(map.erase(none_));
    EXPECT_EQ(1, map.size());
}

TEST_F(demultiplexer_ordered_mime_map_test,
       erase_two_existent_when_two_are_inserted)
{
    demultiplexer_ordered_mime_map map;
    map.insert(none_, request_handler_callback());
    map.insert(another_, request_handler_callback());
    EXPECT_TRUE(map.erase(none_));
    EXPECT_TRUE(map.erase(another_));
    EXPECT_EQ(0, map.size());
}

TEST_F(demultiplexer_ordered_mime_map_test, erase_used)
{
    demultiplexer_ordered_mime_map map;
    map.insert(none_, request_handler_callback());
    map.increase_usage_counter(none_);
    EXPECT_TRUE(map.is_used(none_));
    EXPECT_FALSE(map.erase(none_));
    EXPECT_EQ(map.size(), 1);
    map.decrease_usage_counter(none_);
    EXPECT_FALSE(map.is_used(none_));
    EXPECT_TRUE(map.erase(none_));
    EXPECT_EQ(0, map.size());
}

TEST_F(demultiplexer_ordered_mime_map_test, find_in_empty_vector)
{
    demultiplexer_ordered_mime_map map;
    mime none = none_;
    EXPECT_FALSE(map.find(none));
}

TEST_F(demultiplexer_ordered_mime_map_test, find_none_in_vector)
{
    demultiplexer_ordered_mime_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));

    mime none = none_;
    request_handler_callback none_fn = map.find(none);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_, none);
    EXPECT_EQ(http_status_code::OK, none_fn(request, response));
}

TEST_F(demultiplexer_ordered_mime_map_test, find_none_in_vector_second_time)
{
    demultiplexer_ordered_mime_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(another_, make_request_handler(http_status_code::FOUND));
    map.insert(none_, make_request_handler(http_status_code::OK));

    mime none = none_;
    request_handler_callback none_fn = map.find(none);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_, none);
    EXPECT_EQ(http_status_code::OK, none_fn(request, response));
}

TEST_F(demultiplexer_ordered_mime_map_test, find_wildcard_type_in_vector)
{
    demultiplexer_ordered_mime_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));

    mime wildcard = mime(mime_type::WILDCARD, mime_subtype::NONE);
    request_handler_callback none_fn = map.find(wildcard);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_, wildcard);
    EXPECT_EQ(http_status_code::OK, none_fn(request, response));
}

TEST_F(demultiplexer_ordered_mime_map_test, find_wildcard_subtype_in_vector)
{
    demultiplexer_ordered_mime_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));

    mime wildcard = mime(mime_type::NONE, mime_subtype::WILDCARD);
    request_handler_callback none_fn = map.find(wildcard);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_, wildcard);
    EXPECT_EQ(http_status_code::OK, none_fn(request, response));
}

TEST_F(demultiplexer_ordered_mime_map_test, find_wildcard_in_vector)
{
    demultiplexer_ordered_mime_map map;
    request_interface_mock request;
    response_interface_mock response;
    map.insert(none_, make_request_handler(http_status_code::OK));

    mime wildcard = mime(mime_type::WILDCARD, mime_subtype::WILDCARD);
    request_handler_callback none_fn = map.find(wildcard);

    ASSERT_TRUE(!!none_fn);
    EXPECT_EQ(none_, wildcard);
    EXPECT_EQ(http_status_code::OK, none_fn(request, response));
}

TEST_F(demultiplexer_ordered_mime_map_test, find_unavailable)
{
    demultiplexer_ordered_mime_map map;
    map.insert(none_, make_request_handler(http_status_code::OK));
    map.set_availability(none_, false);

    EXPECT_FALSE(map.is_available(none_));
    mime none = none_;
    request_handler_callback none_fn = map.find(none);
    EXPECT_TRUE(!none_fn);
    EXPECT_EQ(none_, none);
    map.set_availability(none_, true);
    EXPECT_TRUE(map.is_available(none_));
    none_fn = map.find(none);
    EXPECT_TRUE(!!none_fn);
    EXPECT_EQ(none_, none);
}

} // namespace hutzn
