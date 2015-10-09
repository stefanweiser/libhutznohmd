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

#include <utility/trie.hpp>

using namespace testing;

namespace hutzn
{

using trie_ = trie<int32_t>;
using trie_find_result_ = trie_find_result<int32_t>;

trie_find_result_ make_trie_find_result(const size_t used_size,
                                        const int32_t value)
{
    return trie_find_result_{used_size, value};
}

bool operator==(const trie_find_result_& lhs, const trie_find_result_& rhs)
{
    return (lhs.used_size() == rhs.used_size()) && (lhs.value() == rhs.value());
}

TEST(trie, initial_trie_node)
{
    trie_ t{false};
    static const std::string str1 = "abc";
    EXPECT_EQ(make_trie_find_result(0, 0), t.find(str1.c_str(), 3));
}

TEST(trie, insert_and_remove_node)
{
    trie_ t{false};
    static const std::string str1 = "abc";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_FALSE(t.erase("ab"));
    EXPECT_FALSE(t.erase("abcd"));
    EXPECT_TRUE(t.erase(str1.c_str()));
    EXPECT_EQ(make_trie_find_result(0, 0), t.find(str1.c_str(), 3));
}

TEST(trie, find_divergent_strings)
{
    trie_ t{false};
    static const std::string str1 = "abc";
    static const std::string str2 = "def";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_TRUE(t.insert(str2.c_str(), 2));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("abc", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("abcd", 4));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("abcdef", 6));
    EXPECT_EQ(make_trie_find_result(str2.length(), 2), t.find("def", 3));
    EXPECT_EQ(make_trie_find_result(str2.length(), 2), t.find("defg", 4));
    EXPECT_EQ(make_trie_find_result(str2.length(), 2), t.find("defgh", 6));
    EXPECT_FALSE(t.erase("abcdef"));
    EXPECT_TRUE(t.erase("abc"));
    EXPECT_FALSE(t.erase("defghi"));
    EXPECT_TRUE(t.erase("def"));
}

TEST(trie, reinsert)
{
    trie_ t{false};
    static const std::string str1 = "abc";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_FALSE(t.insert(str1.c_str(), 1));
    EXPECT_TRUE(t.erase(str1.c_str()));
    EXPECT_FALSE(t.erase(str1.c_str()));
}

TEST(trie, find_part_strings)
{
    trie_ t{false};
    static const std::string str1 = "abc";
    static const std::string str2 = "abcdef";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_TRUE(t.insert(str2.c_str(), 2));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("abc", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("abcd", 4));
    EXPECT_EQ(make_trie_find_result(str2.length(), 2), t.find("abcdef", 6));
    EXPECT_EQ(make_trie_find_result(str2.length(), 2), t.find("abcdefgh", 8));
    EXPECT_TRUE(t.erase(str1.c_str()));
    EXPECT_TRUE(t.erase(str2.c_str()));
}

TEST(trie, case_sensitive_reinsert)
{
    trie_ t{true};
    static const std::string str1 = "abc";
    static const std::string str2 = "ABC";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_FALSE(t.insert(str2.c_str(), 2));
    EXPECT_TRUE(t.erase(str2.c_str()));
    EXPECT_FALSE(t.erase(str1.c_str()));
}

TEST(trie, case_sensitive_find)
{
    trie_ t{true};
    static const std::string str1 = "abc";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("abc", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("Abc", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("aBc", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("abC", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("aBC", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("AbC", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("ABc", 3));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("ABC", 3));
    EXPECT_TRUE(t.erase(str1.c_str()));
}

TEST(trie, case_sensitive_special_characters)
{
    trie_ t{true};
    static const std::string str1 = "a b c";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("a b c", 5));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("A b c", 5));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("a B c", 5));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("a b C", 5));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("a B C", 5));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("A b C", 5));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("A B c", 5));
    EXPECT_EQ(make_trie_find_result(str1.length(), 1), t.find("A B C", 5));
    EXPECT_TRUE(t.erase(str1.c_str()));
}

} // namespace hutzn
