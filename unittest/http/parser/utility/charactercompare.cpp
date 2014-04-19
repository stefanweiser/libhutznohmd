/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <cstring>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/parser/utility/charactercompare.hpp>

using namespace testing;

namespace rest
{

namespace http
{

namespace
{

LOWER_CASE_STRING(abcd);

} // namespace

TEST(charactercompare, lower_case_string)
{
    EXPECT_EQ(strcmp(lower_case_string_abcd::value, "abcd"), 0);
    EXPECT_EQ(lower_case_string_abcd::size, strlen("abcd"));
}

TEST(charactercompare, to_lower)
{
    const std::array<uint8_t, 256> validity_map = {
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
            0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
            0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
            0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
            0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
            0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
            0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
            0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
            0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
            0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
            0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
            0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
        }
    };

    for (size_t i = 0; i < validity_map.size(); i++) {
        EXPECT_EQ(static_cast<uint8_t>(to_lower(static_cast<char>(i))), validity_map[i]);
    }
}

TEST(charactercompare, url_characters)
{
    std::array<bool, 256> validity_map;
    std::fill(validity_map.begin(), validity_map.end(), false);
    for (char c = 'A'; c <= 'Z'; c++) {
        validity_map[c] = true;
    }
    for (char c = 'a'; c <= 'z'; c++) {
        validity_map[c] = true;
    }
    for (char c = '0'; c <= '9'; c++) {
        validity_map[c] = true;
    }
    validity_map['-'] = true;
    validity_map['_'] = true;
    validity_map['.'] = true;
    validity_map['~'] = true;
    validity_map['!'] = true;
    validity_map['*'] = true;
    validity_map['\''] = true;
    validity_map['('] = true;
    validity_map[')'] = true;
    validity_map[';'] = true;
    validity_map[':'] = true;
    validity_map['@'] = true;
    validity_map['&'] = true;
    validity_map['='] = true;
    validity_map['+'] = true;
    validity_map['$'] = true;
    validity_map[','] = true;
    validity_map['/'] = true;
    validity_map['?'] = true;
    validity_map['%'] = true;
    validity_map['#'] = true;
    validity_map['['] = true;
    validity_map[']'] = true;

    for (size_t i = 0; i < validity_map.size(); i++) {
        EXPECT_EQ(is_valid_url_character(static_cast<uint8_t>(i)), validity_map[i]);
    }
}

TEST(charactercompare, header_key_characters)
{
    std::array<bool, 256> validity_map;
    // Any CHAR
    for (size_t i = 0; i < validity_map.size(); i++) {
        validity_map[i] = (i < 0x80) ? true : false;
    }
    // But no CTLs
    for (size_t i = 0; i <= 31; i++) {
        validity_map[i] = false;
    }
    validity_map[127] = false;

    // And no seperators
    std::vector<uint8_t> seperators = {'(', ')', '<', '>', '@', ',', ';', ':', '\\', '\'', '/',
                                       '[', ']', '?', '=', '{', '}', ' ', '\t'
                                      };
    for (const uint8_t & c : seperators) {
        validity_map[c] = false;
    }

    for (size_t i = 0; i < validity_map.size(); i++) {
        EXPECT_EQ(is_valid_header_key_character(static_cast<uint8_t>(i)), validity_map[i]);
    }
}

TEST(charactercompare, header_value_characters)
{
    std::array<bool, 256> validity_map;
    // Any CHAR
    for (size_t i = 0; i < validity_map.size(); i++) {
        validity_map[i] = true;
    }
    // Instead of all CTLs like in the RFC, we are more tolerant and forbid only the line ending
    // characters. We will not include LWS here like the RFC, because this is already filtered out
    // by the lexer.
    validity_map['\r'] = false;
    validity_map['\n'] = false;

    for (size_t i = 0; i < validity_map.size(); i++) {
        EXPECT_EQ(is_valid_header_value_character(static_cast<uint8_t>(i)), validity_map[i]);
    }
}

} // namespace http

} // namespace rest
