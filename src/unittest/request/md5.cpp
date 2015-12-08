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

#include "request/md5.hpp"

using namespace testing;

namespace hutzn
{

TEST(md5, empty)
{
    const md5_array digest = calculate_md5(std::vector<char_t>());
    const md5_array sum{{0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04, 0xE9,
                         0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E}};
    EXPECT_EQ(sum, digest);
}

TEST(md5, hello_world)
{
    const md5_array digest = calculate_md5(
        {{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'}});
    const md5_array sum{{0xED, 0x07, 0x62, 0x87, 0x53, 0x2E, 0x86, 0x36, 0x5E,
                         0x84, 0x1E, 0x92, 0xBF, 0xC5, 0x0D, 0x8C}};
    EXPECT_EQ(sum, digest);
}

TEST(md5, pangram)
{
    const md5_array digest = calculate_md5(
        {{'P', 'a', 'c', 'k', ' ', 'm', 'y', ' ', 'b', 'o', 'x', ' ', 'w', 'i',
          't', 'h', ' ', 'f', 'i', 'v', 'e', ' ', 'd', 'o', 'z', 'e', 'n', ' ',
          'l', 'i', 'q', 'u', 'o', 'r', ' ', 'j', 'u', 'g', 's'}});
    const md5_array sum{{0x1C, 0x50, 0xC8, 0x5D, 0xF8, 0xC0, 0x2C, 0x99, 0xBC,
                         0x16, 0x05, 0xF6, 0x6E, 0x11, 0xB0, 0x4A}};
    EXPECT_EQ(sum, digest);
}

TEST(md5, long_data)
{
    const md5_array digest = calculate_md5(std::vector<char_t>(80, '0'));
    const md5_array sum{{0xEC, 0x81, 0x7C, 0x0C, 0x0B, 0xE8, 0xA4, 0x11, 0x97,
                         0x83, 0xF8, 0xD0, 0xAF, 0xDC, 0x88, 0x14}};
    EXPECT_EQ(sum, digest);
}

TEST(md5, nearly_full_block)
{
    const md5_array digest = calculate_md5(std::vector<char_t>(60, '0'));
    const md5_array sum{{0x5B, 0x19, 0x44, 0x5B, 0x70, 0xB4, 0x93, 0xC7, 0x8F,
                         0x3B, 0xC0, 0x6E, 0xB7, 0x96, 0x23, 0x15}};
    EXPECT_EQ(sum, digest);
}

} // namespace hutzn
