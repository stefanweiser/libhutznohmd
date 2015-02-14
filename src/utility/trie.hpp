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

#ifndef LIBHUTZNOHMD_UTILITY_TRIE_HPP
#define LIBHUTZNOHMD_UTILITY_TRIE_HPP

#include <array>
#include <memory>
#include <tuple>
#include <vector>

#include <utility/common.hpp>

namespace hutzn
{

namespace detail
{

template <typename value_type>
struct trie_node
{
    using self_ = trie_node<value_type>;

    bool has_value_{false};
    value_type value_{value_type()};
    std::array<std::unique_ptr<self_>, 256> children_;
    size_t used_children_{0};
};

} // namespace detail

template <typename value_type>
class trie
{
public:
    trie(const bool is_case_insensitive)
        : is_case_insensitive_(is_case_insensitive)
        , root_node_{}
    {
    }

    bool insert(const char* string, const value_type& value)
    {
        return insert(root_node_, string, value);
    }

    bool erase(const char* string)
    {
        return erase(root_node_, string);
    }

private:
    using trie_node_ = detail::trie_node<value_type>;

    trie_node_& get_or_create_child(trie_node_& curr, const uint8_t c)
    {
        if (!curr.children_[c]) {
            curr.children_[c] = std::unique_ptr<trie_node_>(new trie_node_);
            curr.used_children_++;
        }
        return *curr.children_[c];
    }

    bool insert(trie_node_& curr, const char* string, const value_type& value)
    {
        const uint8_t c = static_cast<uint8_t>(*string);
        if (c == '\0') {

            // Check if node is already possessed.
            if (true == curr.has_value_) {
                return false;
            }

            curr.has_value_ = true;
            curr.value_ = value;

        } else {

            if (false ==
                insert(get_or_create_child(curr, c), string++, value)) {
                return false;
            }

            if (true == is_case_insensitive_) {
                if (true == check_range<uint8_t, 'a', 'z'>(c)) {

                    if (false == insert(get_or_create_child(curr, c & 0xDFU),
                                        string++, value)) {
                        return false;
                    }

                } else if (true == check_range<uint8_t, 'A', 'Z'>(c)) {

                    if (false == insert(get_or_create_child(curr, c | 0x20U),
                                        string++, value)) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    bool erase(trie_node_& curr, const char* string)
    {
        const uint8_t c = static_cast<uint8_t>(*string);
        if (c == '\0') {

            // Check if node is not possessed.
            if (false == curr.has_value_) {
                return false;
            }

            curr.has_value_ = false;

        } else {

            std::unique_ptr<trie_node_>& child = curr.children_[c];
            if (!child || (false == erase(*child, string++))) {
                return false;
            }

            if ((0 == child->used_children_) && (false == child->has_value_)) {
                child.reset();
            }

            if (true == is_case_insensitive_) {
                if (true == check_range<uint8_t, 'a', 'z'>(c)) {

                    std::unique_ptr<trie_node_>& other_child =
                        curr.children_[c & 0xDFU];
                    if (!other_child ||
                        (false == erase(*other_child, string++))) {
                        return false;
                    }

                    if ((0 == other_child->used_children_) &&
                        (false == other_child->has_value_)) {
                        other_child.reset();
                    }

                } else if (true == check_range<uint8_t, 'A', 'Z'>(c)) {

                    std::unique_ptr<trie_node_>& other_child =
                        curr.children_[c | 0x20U];
                    if (!other_child ||
                        (false == erase(*other_child, string++))) {
                        return false;
                    }

                    if ((0 == other_child->used_children_) &&
                        (false == other_child->has_value_)) {
                        other_child.reset();
                    }
                }
            }
        }

        return true;
    }

    const bool is_case_insensitive_;
    detail::trie_node<value_type> root_node_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_TRIE_HPP
