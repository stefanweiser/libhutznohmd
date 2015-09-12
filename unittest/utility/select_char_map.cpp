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

#include <utility/select_char_map.hpp>

using namespace testing;

namespace hutzn
{

TEST(select_char_map, empty_select_char_map)
{
    select_char_map expectation;
    EXPECT_EQ(expectation, make_select_char_map());
}

TEST(select_char_map, select_char_map_filled_with_one_character)
{
    select_char_map expectation;
    expectation[' '] = true;
    EXPECT_EQ(expectation, make_select_char_map(' '));
}

TEST(select_char_map, select_char_map_filled_with_two_characters)
{
    select_char_map expectation;
    expectation[' '] = true;
    expectation['\t'] = true;
    EXPECT_EQ(expectation, make_select_char_map(' ', '\t'));
}

} // namespace hutzn
