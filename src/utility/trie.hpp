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

#ifndef LIBHUTZNOHMD_UTILITY_TRIE_HPP
#define LIBHUTZNOHMD_UTILITY_TRIE_HPP

#include <array>
#include <memory>
#include <tuple>
#include <vector>

#include <utility/common.hpp>

namespace hutzn
{

//! Implements a trie to map a string to a specific value. It could be case
//! sensitive or case insensitive and provides the basic operations insert, find
//! and erase. The find operation will search for stored trie tokens at the
//! search string's begin. It will always find the longest result.
template <typename value_type>
class trie
{
public:
    //! Result structure for each find operation.
    struct find_result_type
    {
        //! Number of characters, that were used to get a valid token. Is zero
        //! if no token was found (the value is undefined then).
        size_t used_size;

        //! Found value or undefined if no value was found.
        value_type value;

        bool operator==(const find_result_type& rhs) const
        {
            return (rhs.used_size == used_size) && (rhs.value == value);
        }
    };

    static_assert(sizeof(uint8_t) == sizeof(char),
                  "The trie implementation need a char type that has 8 bits or "
                  "it would compromise some type convertions.");

    //! Determines whether the trie and all it's operations are acting case
    //! sensitive or not.
    trie(const bool is_case_insensitive)
        : is_case_insensitive_(is_case_insensitive)
        , root_node_()
    {
    }

    //! Returns the longest match inside the trie. At most max_length characters
    //! are read. It returns a structure of used length to get this token and
    //! result value. If no token is found, the result's used size is zero.
    find_result_type find(const char* const search_string,
                          const size_t max_length) const
    {
        return find(root_node_, search_string, search_string, max_length);
    }

    //! Inserts a token with it's value into the trie. Returns whether the token
    //! was not already in the trie and the token was therefore inserted.
    bool insert(const char* token, const value_type& value)
    {
        return insert(root_node_, token, value);
    }

    //! Erases a token. Returns whether the token could be erased.
    bool erase(const char* token)
    {
        return erase(root_node_, token);
    }

private:
    struct trie_node
    {
        trie_node()
            : has_value(false)
            , value()
            , children()
            , used_children(0)
        {
        }

        bool has_value;
        value_type value;
        std::array<std::unique_ptr<trie_node>, 256> children;
        size_t used_children;
    };

    find_result_type generate_find_result(const trie_node& node,
                                          const char* const original_string,
                                          const char* const string) const
    {
        const size_t used_chars =
            node.has_value ? static_cast<size_t>(string - original_string) : 0;
        return find_result_type{used_chars, node.value};
    }

    find_result_type find(const trie_node& node,
                          const char* const original_search_string,
                          const char* const search_string,
                          const size_t characters_remaining) const
    {
        if (0 == characters_remaining) {
            return generate_find_result(node, original_search_string,
                                        search_string);
        }

        find_result_type result{0, value_type()};

        const std::unique_ptr<trie_node>& child =
            node.children[static_cast<uint8_t>(*search_string)];
        if (child) {
            result = find(*child, original_search_string, search_string + 1,
                          characters_remaining - 1);
        }

        if (0 == result.used_size) {
            return generate_find_result(node, original_search_string,
                                        search_string);
        }
        return result;
    }

    trie_node& get_or_create_child(trie_node& curr, const uint8_t c)
    {
        if (!curr.children[c]) {
            curr.children[c] = std::unique_ptr<trie_node>(new trie_node);
            curr.used_children++;
        }
        return *curr.children[c];
    }

    bool insert(trie_node& curr, const char* token, const value_type& value)
    {
        const uint8_t c = static_cast<uint8_t>(*token);
        if (c == 0) {

            // Check if node is already possessed.
            if (true == curr.has_value) {
                return false;
            }

            curr.has_value = true;
            curr.value = value;

        } else {

            if (false ==
                insert(get_or_create_child(curr, c), token + 1, value)) {
                return false;
            }

            if (true == is_case_insensitive_) {
                if (true == check_range<uint8_t, 'a', 'z'>(c)) {

                    if (false == insert(get_or_create_child(curr, c & 0xDFU),
                                        token + 1, value)) {
                        return false;
                    }

                } else if (true == check_range<uint8_t, 'A', 'Z'>(c)) {

                    if (false == insert(get_or_create_child(curr, c | 0x20U),
                                        token + 1, value)) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    bool erase(trie_node& curr, const char* token)
    {
        const uint8_t c = static_cast<uint8_t>(*token);
        if (c == 0) {

            // Check if node is not possessed.
            if (false == curr.has_value) {
                return false;
            }

            curr.has_value = false;

        } else {

            std::unique_ptr<trie_node>& child = curr.children[c];
            if (!child || (false == erase(*child, token + 1))) {
                return false;
            }

            if ((0 == child->used_children) && (false == child->has_value)) {
                child.reset();
            }

            if (true == is_case_insensitive_) {
                if (true == check_range<uint8_t, 'a', 'z'>(c)) {

                    std::unique_ptr<trie_node>& other_child =
                        curr.children[c & 0xDFU];
                    if (!other_child ||
                        (false == erase(*other_child, token + 1))) {
                        return false;
                    }

                    if ((0 == other_child->used_children) &&
                        (false == other_child->has_value)) {
                        other_child.reset();
                    }

                } else if (true == check_range<uint8_t, 'A', 'Z'>(c)) {

                    std::unique_ptr<trie_node>& other_child =
                        curr.children[c | 0x20U];
                    if (!other_child ||
                        (false == erase(*other_child, token + 1))) {
                        return false;
                    }

                    if ((0 == other_child->used_children) &&
                        (false == other_child->has_value)) {
                        other_child.reset();
                    }
                }
            }
        }

        return true;
    }

    const bool is_case_insensitive_;
    trie_node root_node_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_TRIE_HPP
