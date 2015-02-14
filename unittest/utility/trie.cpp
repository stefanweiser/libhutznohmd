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
    EXPECT_EQ(t.find("abc", 3), std::make_tuple(false, 0));
}

TEST(dynamic_trie, insert_and_remove_node)
{
    trie<int> t = trie<int>(false);
    EXPECT_TRUE(t.insert("abc", 1));
    EXPECT_FALSE(t.erase("ab"));
    EXPECT_FALSE(t.erase("abcd"));
    EXPECT_TRUE(t.erase("abc"));
    EXPECT_EQ(t.find("abc", 3), std::make_tuple(false, 0));
}

TEST(dynamic_trie, find_divergent_strings)
{
    trie<int> t = trie<int>(false);
    EXPECT_TRUE(t.insert("abc", 1));
    EXPECT_TRUE(t.insert("def", 2));
    EXPECT_EQ(t.find("abc", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("abcd", 4), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("abcdef", 6), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("def", 3), std::make_tuple(true, 2));
    EXPECT_EQ(t.find("defg", 4), std::make_tuple(true, 2));
    EXPECT_EQ(t.find("defgh", 6), std::make_tuple(true, 2));
    EXPECT_FALSE(t.erase("abcdef"));
    EXPECT_TRUE(t.erase("abc"));
    EXPECT_FALSE(t.erase("defghi"));
    EXPECT_TRUE(t.erase("def"));
}

TEST(dynamic_trie, reinsert)
{
    trie<int> t = trie<int>(false);
    EXPECT_TRUE(t.insert("abc", 1));
    EXPECT_FALSE(t.insert("abc", 1));
    EXPECT_TRUE(t.erase("abc"));
    EXPECT_FALSE(t.erase("abc"));
}

TEST(dynamic_trie, find_part_strings)
{
    trie<int> t = trie<int>(false);
    EXPECT_TRUE(t.insert("abc", 1));
    EXPECT_TRUE(t.insert("abcdef", 2));
    EXPECT_EQ(t.find("abc", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("abcd", 4), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("abcdef", 6), std::make_tuple(true, 2));
    EXPECT_TRUE(t.erase("abc"));
    EXPECT_TRUE(t.erase("abcdef"));
}

TEST(dynamic_trie, case_sensitive_reinsert)
{
    trie<int> t = trie<int>(true);
    EXPECT_TRUE(t.insert("abc", 1));
    EXPECT_FALSE(t.insert("ABC", 2));
    EXPECT_TRUE(t.erase("ABC"));
    EXPECT_FALSE(t.erase("abc"));
}

TEST(dynamic_trie, case_sensitive_find)
{
    trie<int> t = trie<int>(true);
    EXPECT_TRUE(t.insert("abc", 1));
    EXPECT_EQ(t.find("abc", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("Abc", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("aBc", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("abC", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("aBC", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("AbC", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("ABc", 3), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("ABC", 3), std::make_tuple(true, 1));
    EXPECT_TRUE(t.erase("abc"));
}

TEST(dynamic_trie, case_sensitive_special_characters)
{
    trie<int> t = trie<int>(true);
    EXPECT_TRUE(t.insert("a b c", 1));
    EXPECT_EQ(t.find("a b c", 5), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("A b c", 5), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("a B c", 5), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("a b C", 5), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("a B C", 5), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("A b C", 5), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("A B c", 5), std::make_tuple(true, 1));
    EXPECT_EQ(t.find("A B C", 5), std::make_tuple(true, 1));
    EXPECT_TRUE(t.erase("a b c"));
}

} // namespace hutzn
