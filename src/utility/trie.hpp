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
#include <cstdint>
#include <memory>

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
                  "The trie implementation needs a char type that has 8 bits or"
                  " it would compromise some type convertions.");

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
        return root_node_.find(search_string, search_string, max_length);
    }

    //! Inserts a token with it's value into the trie. Returns whether the token
    //! was not already in the trie and the token was therefore inserted.
    bool insert(const char* token, const value_type& value)
    {
        return root_node_.insert(token, value, is_case_insensitive_);
    }

    //! Erases a token. Returns whether the token could be erased.
    bool erase(const char* token)
    {
        return root_node_.erase(token, is_case_insensitive_);
    }

private:
    class trie_node
    {
    public:
        trie_node()
            : has_value_(false)
            , value_()
            , children_()
            , used_children_(0)
        {
        }

        find_result_type make_find_result(const char* const begin,
                                          const char* const curr) const
        {
            const size_t distance = static_cast<size_t>(curr - begin);
            const size_t used_chars = has_value_ ? distance : 0;
            return find_result_type{used_chars, value_};
        }

        find_result_type find(const char* const original,
                              const char* const curr,
                              const size_t remaining) const
        {
            if (0 == remaining) {
                return make_find_result(original, curr);
            }

            find_result_type result{0, value_type()};

            const auto& child = children_[static_cast<uint8_t>(*curr)];
            if (child) {
                result = child->find(original, curr + 1, remaining - 1);
            }

            if (0 == result.used_size) {
                return make_find_result(original, curr);
            }
            return result;
        }

        bool insert(const char* token, const value_type& value,
                    const bool is_case_insensitive)
        {
            const uint8_t c = static_cast<uint8_t>(*token);
            if (c == 0) {

                // Check if node is already possessed.
                if (true == has_value_) {
                    return false;
                }

                has_value_ = true;
                value_ = value;

            } else {

                const char* next = token + 1;

                if (!children_[c]) {
                    children_[c] = std::make_shared<trie_node>();
                    used_children_++;
                }
                auto& child = children_[c];

                if (false == child->insert(next, value, is_case_insensitive)) {
                    return false;
                }

                if (true == is_case_insensitive) {
                    if (true == check_range<uint8_t, 'a', 'z'>(c)) {
                        auto& other = children_[c & 0xDFU];
                        if (nullptr == other.get()) {
                            other = child;
                            used_children_++;
                        }
                    } else if (true == check_range<uint8_t, 'A', 'Z'>(c)) {
                        auto& other = children_[c | 0x20U];
                        if (nullptr == other.get()) {
                            other = child;
                            used_children_++;
                        }
                    }
                }
            }

            return true;
        }

        bool erase(const char* token, const bool is_case_insensitive)
        {
            const uint8_t c = static_cast<uint8_t>(*token);
            if (c == 0) {

                // Check if node is not possessed.
                if (false == has_value_) {
                    return false;
                }

                has_value_ = false;

            } else {

                const char* next = token + 1;

                auto& child = children_[c];
                if (!child ||
                    (false == child->erase(next, is_case_insensitive))) {
                    return false;
                }

                if ((0 == child->used_children_) &&
                    (false == child->has_value_)) {
                    child.reset();
                    used_children_--;
                }

                if (true == is_case_insensitive) {
                    if (true == check_range<uint8_t, 'a', 'z'>(c)) {

                        auto& other_child = children_[c & 0xDFU];
                        if ((0 == other_child->used_children_) &&
                            (false == other_child->has_value_)) {
                            other_child.reset();
                            used_children_--;
                        }

                    } else if (true == check_range<uint8_t, 'A', 'Z'>(c)) {

                        auto& other_child = children_[c | 0x20U];
                        if ((0 == other_child->used_children_) &&
                            (false == other_child->has_value_)) {
                            other_child.reset();
                            used_children_--;
                        }
                    }
                }
            }

            return true;
        }

    private:
        bool has_value_;
        value_type value_;
        std::array<std::shared_ptr<trie_node>, 256> children_;
        size_t used_children_;
    };

    const bool is_case_insensitive_;
    trie_node root_node_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_TRIE_HPP
