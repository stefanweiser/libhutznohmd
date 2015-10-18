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

#include <http/parser/utility/trie.hpp>

using namespace testing;

namespace hutzn
{

namespace http
{

namespace
{

using string_index_pair = std::pair<std::string, size_t>;

int32_t get_char(void* handle)
{
    string_index_pair* p = static_cast<string_index_pair*>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second++]);
    }
    return -1;
}

int32_t peek_char(void* handle)
{
    string_index_pair* p = static_cast<string_index_pair*>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second]);
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string& str);

    template <size_t size>
    size_t parse(const std::vector<trie<size_t>::value_info>& strings,
                 push_back_string<size>& fail_safe_result);

    std::string str_;
};

fixture::fixture(const std::string& str)
    : str_(str)
{
}

template <size_t size>
size_t fixture::parse(const std::vector<trie<size_t>::value_info>& values,
                      push_back_string<size>& fail_safe_result)
{
    string_index_pair p(str_, 0);
    lexer lex(anonymous_int_function(&get_char, &p),
              anonymous_int_function(&peek_char, &p));
    trie<size_t> test_trie(values, 0);
    int32_t character = lex.get();
    return test_trie.parse(character, fail_safe_result, lex);
}

} // namespace

TEST(static_trie, basic_function)
{
    fixture f("abc");
    push_back_string<4> fail_safe_result;
    std::vector<trie<size_t>::value_info> values = {
        {trie<size_t>::value_info{"abc", 1}, trie<size_t>::value_info{"def", 2},
         trie<size_t>::value_info{"aef", 3}}};
    EXPECT_EQ(1, f.parse(values, fail_safe_result));
    EXPECT_EQ(std::string(""), fail_safe_result.c_str());
}

TEST(static_trie, basic_function2)
{
    fixture f("aef");
    push_back_string<4> fail_safe_result;
    std::vector<trie<size_t>::value_info> values = {
        {trie<size_t>::value_info{"abc", 1}, trie<size_t>::value_info{"def", 2},
         trie<size_t>::value_info{"aef", 3}}};
    EXPECT_EQ(3, f.parse(values, fail_safe_result));
    EXPECT_EQ(std::string(""), fail_safe_result.c_str());
}

TEST(static_trie, failed)
{
    fixture f("aef");
    push_back_string<4> fail_safe_result;
    std::vector<trie<size_t>::value_info> values = {
        {trie<size_t>::value_info{"aBc", 1}, trie<size_t>::value_info{"def", 2},
         trie<size_t>::value_info{"aef", 3}}};
    EXPECT_EQ(3, f.parse(values, fail_safe_result));
    EXPECT_EQ(std::string(""), fail_safe_result.c_str());
}

} // namespace http

} // namespace hutzn
