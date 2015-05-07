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

} // namespace hutzn
