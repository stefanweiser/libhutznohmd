/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

TEST(dynamic_trie, initial_trie_node)
{
    trie<int> t = trie<int>(false);
    static const std::string str1 = "abc";
    EXPECT_EQ(t.find(str1.c_str(), 3), std::make_tuple(0, 0));
}

TEST(dynamic_trie, insert_and_remove_node)
{
    trie<int> t = trie<int>(false);
    static const std::string str1 = "abc";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_FALSE(t.erase("ab"));
    EXPECT_FALSE(t.erase("abcd"));
    EXPECT_TRUE(t.erase(str1.c_str()));
    EXPECT_EQ(t.find(str1.c_str(), 3), std::make_tuple(0, 0));
}

TEST(dynamic_trie, find_divergent_strings)
{
    trie<int> t = trie<int>(false);
    static const std::string str1 = "abc";
    static const std::string str2 = "def";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_TRUE(t.insert(str2.c_str(), 2));
    EXPECT_EQ(t.find("abc", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("abcd", 4), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("abcdef", 6), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("def", 3), std::make_tuple(str2.length(), 2));
    EXPECT_EQ(t.find("defg", 4), std::make_tuple(str2.length(), 2));
    EXPECT_EQ(t.find("defgh", 6), std::make_tuple(str2.length(), 2));
    EXPECT_FALSE(t.erase("abcdef"));
    EXPECT_TRUE(t.erase("abc"));
    EXPECT_FALSE(t.erase("defghi"));
    EXPECT_TRUE(t.erase("def"));
}

TEST(dynamic_trie, reinsert)
{
    trie<int> t = trie<int>(false);
    static const std::string str1 = "abc";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_FALSE(t.insert(str1.c_str(), 1));
    EXPECT_TRUE(t.erase(str1.c_str()));
    EXPECT_FALSE(t.erase(str1.c_str()));
}

TEST(dynamic_trie, find_part_strings)
{
    trie<int> t = trie<int>(false);
    static const std::string str1 = "abc";
    static const std::string str2 = "abcdef";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_TRUE(t.insert(str2.c_str(), 2));
    EXPECT_EQ(t.find("abc", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("abcd", 4), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("abcdef", 6), std::make_tuple(str2.length(), 2));
    EXPECT_EQ(t.find("abcdefgh", 8), std::make_tuple(str2.length(), 2));
    EXPECT_TRUE(t.erase(str1.c_str()));
    EXPECT_TRUE(t.erase(str2.c_str()));
}

TEST(dynamic_trie, case_sensitive_reinsert)
{
    trie<int> t = trie<int>(true);
    static const std::string str1 = "abc";
    static const std::string str2 = "ABC";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_FALSE(t.insert(str2.c_str(), 2));
    EXPECT_TRUE(t.erase(str2.c_str()));
    EXPECT_FALSE(t.erase(str1.c_str()));
}

TEST(dynamic_trie, case_sensitive_find)
{
    trie<int> t = trie<int>(true);
    static const std::string str1 = "abc";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_EQ(t.find("abc", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("Abc", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("aBc", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("abC", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("aBC", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("AbC", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("ABc", 3), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("ABC", 3), std::make_tuple(str1.length(), 1));
    EXPECT_TRUE(t.erase(str1.c_str()));
}

TEST(dynamic_trie, case_sensitive_special_characters)
{
    trie<int> t = trie<int>(true);
    static const std::string str1 = "a b c";
    EXPECT_TRUE(t.insert(str1.c_str(), 1));
    EXPECT_EQ(t.find("a b c", 5), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("A b c", 5), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("a B c", 5), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("a b C", 5), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("a B C", 5), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("A b C", 5), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("A B c", 5), std::make_tuple(str1.length(), 1));
    EXPECT_EQ(t.find("A B C", 5), std::make_tuple(str1.length(), 1));
    EXPECT_TRUE(t.erase(str1.c_str()));
}

} // namespace hutzn
