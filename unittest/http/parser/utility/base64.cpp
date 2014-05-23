/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/parser/utility/base64.hpp>

#include <rest.hpp>

using namespace testing;

namespace rest
{

namespace http
{

TEST(base64, encode_empty)
{
    std::string base64 = encode_base64({});
    EXPECT_EQ(base64, "");
}

TEST(base64, encode_space)
{
    std::string base64 = encode_base64({' '});
    EXPECT_EQ(base64, "IA==");
}

TEST(base64, encode_hello_world)
{
    std::string base64 = encode_base64({
        'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'
    });
    EXPECT_EQ(base64, "SGVsbG8gV29ybGQh");
}

TEST(base64, encode_pangram)
{
    std::string base64 = encode_base64({
        'P', 'u', 'b', 'l', 'i', 'c', ' ', 'j', 'u', 'n', 'k', ' ', 'd', 'w', 'a', 'r', 'v', 'e',
        's', ' ', 'h', 'u', 'g', ' ', 'm', 'y', ' ', 'q', 'u', 'a', 'r', 't', 'z', ' ', 'f', 'o',
        'x', '.'
    });
    EXPECT_EQ(base64, "UHVibGljIGp1bmsgZHdhcnZlcyBodWcgbXkgcXVhcnR6IGZveC4=");
}

TEST(base64, decode_empty)
{
    std::vector<uint8_t> decoded_data = decode_base64("");
    std::vector<uint8_t> data;
    EXPECT_EQ(decoded_data, data);
}

TEST(base64, decode_space)
{
    std::vector<uint8_t> decoded_data = decode_base64("IA==");
    std::vector<uint8_t> data {' '};
    EXPECT_EQ(decoded_data, data);
}

TEST(base64, decode_double_space)
{
    std::vector<uint8_t> decoded_data = decode_base64("ICA=");
    std::vector<uint8_t> data {' ', ' '};
    EXPECT_EQ(decoded_data, data);
}

TEST(base64, decode_space_without_pad)
{
    std::vector<uint8_t> decoded_data = decode_base64("IA");
    std::vector<uint8_t> data {' '};
    EXPECT_EQ(decoded_data, data);
}

TEST(base64, decode_space_with_ignored_stuff)
{
    std::vector<uint8_t> decoded_data = decode_base64("I---A==");
    std::vector<uint8_t> data {' '};
    EXPECT_EQ(decoded_data, data);
}

TEST(base64, decode_hello_world)
{
    std::vector<uint8_t> decoded_data = decode_base64("SGVsbG8gV29ybGQh");
    std::vector<uint8_t> data {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    EXPECT_EQ(decoded_data, data);
}

TEST(base64, decode_errornous)
{
    std::vector<uint8_t> decoded_data = decode_base64("I");
    EXPECT_EQ(decoded_data, std::vector<uint8_t>());
}

} // namespace http

} // namespace rest
