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

#include "utility/character_validation.hpp"

using namespace testing;

namespace hutzn
{

using to_lower_test_param = std::tuple<char_t, char_t>;

class to_lower_test : public TestWithParam<to_lower_test_param>
{
};

std::vector<to_lower_test_param> get_all_to_lower_test_params()
{
    std::vector<to_lower_test_param> result;
    for (size_t i = 0; i < byte_state_count; i++) {
        const char_t c = static_cast<char_t>(i);
        result.push_back(std::make_tuple(c, c));
    }
    result['A'] = std::make_tuple('A', 'a');
    result['B'] = std::make_tuple('B', 'b');
    result['C'] = std::make_tuple('C', 'c');
    result['D'] = std::make_tuple('D', 'd');
    result['E'] = std::make_tuple('E', 'e');
    result['F'] = std::make_tuple('F', 'f');
    result['G'] = std::make_tuple('G', 'g');
    result['H'] = std::make_tuple('H', 'h');
    result['I'] = std::make_tuple('I', 'i');
    result['J'] = std::make_tuple('J', 'j');
    result['K'] = std::make_tuple('K', 'k');
    result['L'] = std::make_tuple('L', 'l');
    result['M'] = std::make_tuple('M', 'm');
    result['N'] = std::make_tuple('N', 'n');
    result['O'] = std::make_tuple('O', 'o');
    result['P'] = std::make_tuple('P', 'p');
    result['Q'] = std::make_tuple('Q', 'q');
    result['R'] = std::make_tuple('R', 'r');
    result['S'] = std::make_tuple('S', 's');
    result['T'] = std::make_tuple('T', 't');
    result['U'] = std::make_tuple('U', 'u');
    result['V'] = std::make_tuple('V', 'v');
    result['W'] = std::make_tuple('W', 'w');
    result['X'] = std::make_tuple('X', 'x');
    result['Y'] = std::make_tuple('Y', 'y');
    result['Z'] = std::make_tuple('Z', 'z');
    return result;
}

INSTANTIATE_TEST_SUITE_P(character_handling, to_lower_test,
                         ValuesIn(get_all_to_lower_test_params()));

TEST_P(to_lower_test, to_lower)
{
    char_t input;
    char_t output;
    std::tie(input, output) = GetParam();
    EXPECT_EQ(output, to_lower(input));
}

using from_hex_test_param = std::tuple<char_t, uint8_t>;

class from_hex_test : public TestWithParam<from_hex_test_param>
{
};

std::vector<from_hex_test_param> get_all_from_hex_test_params()
{
    std::vector<from_hex_test_param> result;
    for (size_t i = 0; i < byte_state_count; i++) {
        const char_t c = static_cast<char_t>(i);
        result.push_back(std::make_tuple(c, 0xFF));
    }
    result['0'] = std::make_tuple('0', 0);
    result['1'] = std::make_tuple('1', 1);
    result['2'] = std::make_tuple('2', 2);
    result['3'] = std::make_tuple('3', 3);
    result['4'] = std::make_tuple('4', 4);
    result['5'] = std::make_tuple('5', 5);
    result['6'] = std::make_tuple('6', 6);
    result['7'] = std::make_tuple('7', 7);
    result['8'] = std::make_tuple('8', 8);
    result['9'] = std::make_tuple('9', 9);
    result['A'] = std::make_tuple('A', 10);
    result['B'] = std::make_tuple('B', 11);
    result['C'] = std::make_tuple('C', 12);
    result['D'] = std::make_tuple('D', 13);
    result['E'] = std::make_tuple('E', 14);
    result['F'] = std::make_tuple('F', 15);
    result['a'] = std::make_tuple('a', 10);
    result['b'] = std::make_tuple('b', 11);
    result['c'] = std::make_tuple('c', 12);
    result['d'] = std::make_tuple('d', 13);
    result['e'] = std::make_tuple('e', 14);
    result['f'] = std::make_tuple('f', 15);
    return result;
}

INSTANTIATE_TEST_SUITE_P(character_handling, from_hex_test,
                         ValuesIn(get_all_from_hex_test_params()));

TEST_P(from_hex_test, from_hex)
{
    char_t input;
    uint8_t output;
    std::tie(input, output) = GetParam();
    EXPECT_EQ(output, from_hex(input));
}

using url_path_test_param = std::tuple<char_t, bool>;

class url_path_test : public TestWithParam<url_path_test_param>
{
};

std::vector<url_path_test_param> get_all_url_path_test_params()
{
    std::vector<url_path_test_param> result;
    for (size_t i = 0; i < byte_state_count; i++) {
        const char_t c = static_cast<char_t>(i);
        result.push_back(std::make_tuple(c, false));
    }

    for (char_t c = 'A'; c <= 'Z'; c++) {
        result[static_cast<size_t>(c)] = std::make_tuple(c, true);
    }
    for (char_t c = 'a'; c <= 'z'; c++) {
        result[static_cast<size_t>(c)] = std::make_tuple(c, true);
    }
    for (char_t c = '0'; c <= '9'; c++) {
        result[static_cast<size_t>(c)] = std::make_tuple(c, true);
    }
    for (const char_t c :
         {'-', '.', '_', '~', '%', '!', '$', '&', '\'', '(', ')',
          '*', '+', ',', ';', '=', ':', '/', '[', ']',  '@'}) {
        result[static_cast<size_t>(c)] = std::make_tuple(c, true);
    }
    return result;
}

INSTANTIATE_TEST_SUITE_P(character_handling, url_path_test,
                         ValuesIn(get_all_url_path_test_params()));

TEST_P(url_path_test, url_path)
{
    char_t input;
    bool output;
    std::tie(input, output) = GetParam();
    EXPECT_EQ(output, is_valid_url_path_character(input));
}

} // namespace hutzn
