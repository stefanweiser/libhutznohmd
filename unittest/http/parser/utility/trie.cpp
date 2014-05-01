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

#include <http/parser/utility/trie.hpp>

using namespace testing;

namespace rest
{

namespace http
{

namespace
{

typedef std::pair<std::string, size_t> string_index_pair;

int32_t get_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return p->first[p->second++];
    }
    return -1;
}

int32_t peek_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return p->first[p->second];
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string & str);

    template<size_t size>
    size_t parse(const std::vector<trie<size_t>::value_info> & strings,
                 push_back_string<size> & fail_safe_result);

    std::string str_;
};

fixture::fixture(const std::string & str)
    : str_(str)
{}

template<size_t size>
size_t fixture::parse(const std::vector<trie<size_t>::value_info> & values,
                      push_back_string<size> & fail_safe_result)
{
    string_index_pair p(str_, 0);
    lexer l(anonymous_int_function(&get_char, &p),
            anonymous_int_function(&peek_char, &p));
    trie<size_t> t(values, 0);
    int32_t character = l.get();
    return t.parse(character, fail_safe_result, l);
}

} // namespace

TEST(trie_parse, basic_function)
{
    fixture f("abc");
    push_back_string<4> fail_safe_result;
    std::vector<trie<size_t>::value_info> values = {{
            trie<size_t>::value_info{"abc", 1},
            trie<size_t>::value_info{"def", 2},
            trie<size_t>::value_info{"aef", 3}
        }
    };
    EXPECT_EQ(f.parse(values, fail_safe_result), 1);
    EXPECT_EQ(fail_safe_result.c_str(), std::string(""));
}

TEST(trie_parse, basic_function2)
{
    fixture f("abc");
    push_back_string<4> fail_safe_result;
    std::vector<trie<size_t>::value_info> values = {{
            trie<size_t>::value_info{"abc", 1},
            trie<size_t>::value_info{"def", 2},
            trie<size_t>::value_info{"aef", 3}
        }
    };
    EXPECT_EQ(f.parse(values, fail_safe_result), 1);
    EXPECT_EQ(fail_safe_result.c_str(), std::string(""));
}

TEST(trie_parse, failed)
{
    fixture f("aef");
    push_back_string<4> fail_safe_result;
    std::vector<trie<size_t>::value_info> values = {{
            trie<size_t>::value_info{"abc", 1},
            trie<size_t>::value_info{"def", 2},
            trie<size_t>::value_info{"aef", 3}
        }
    };
    EXPECT_EQ(f.parse(values, fail_safe_result), 3);
    EXPECT_EQ(fail_safe_result.c_str(), std::string(""));
}

} // namespace http

} // namespace rest