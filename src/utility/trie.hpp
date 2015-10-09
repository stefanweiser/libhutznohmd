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
#include <cassert>

#include <libhutznohmd/types.hpp>

#include <utility/common.hpp>

namespace hutzn
{

//! Result structure for each find operation.
template <typename value_type>
class trie_find_result
{
public:
    //! @brief Constructs the result of a trie's find operation with no result.
    trie_find_result(void)
        : used_size_(0)
        , value_()
    {
    }

    //! C@brief onstructs the result of a trie's find operation with size and
    //! value.
    trie_find_result(const size_t s, const value_type& v)
        : used_size_(s)
        , value_(v)
    {
    }

    //! @brief Returns the used size attribute.
    size_t used_size(void) const
    {
        return used_size_;
    }

    //! @brief Returns the value attribute.
    const value_type& value(void) const
    {
        return value_;
    }

private:
    //! Number of characters, that were used to get a valid token. Is zero
    //! if no token was found (the value is undefined then).
    size_t used_size_;

    //! Found value or undefined if no value was found.
    value_type value_;
};

namespace detail
{

//! Implements a node of the trie.
template <typename value_type>
class trie_node
{
public:
    //! Specializes the result of the find operation.
    using trie_find_result_ = trie_find_result<value_type>;

    //! @brief Constructs the node.
    explicit trie_node(void)
        : has_value_(false)
        , value_()
        , children_()
        , used_children_(0)
    {
    }

    //! @brief Destroys the node. Deletes its children recursively. Clears the
    //! pointers if they are case sensitive.
    ~trie_node(void)
    {
        // Release all children first.
        for (size_t i = 0; i < children_.size(); i++) {
            uint8_t c = static_cast<uint8_t>(i);
            trie_node* child = children_[c];

            // Delete only if set.
            if (child != nullptr) {
                delete child;

                // Because it could be case sensitive, check whether the
                // corresponding character is set to the same child. If
                // this holds true, then reset that pointer to avoid double
                // deletion. It is important to use the binary to look
                // forward and not backward!
                if (check_range<uint8_t, 'A', 'Z'>(c)) {
                    trie_node*& other = children_[make_lower(c)];
                    clear_right_side_when_equal(child, other);
                }
            }
        }
    }

    //! @brief Makes a find result by calculating the distance between curr and
    //! begin pointer.
    trie_find_result_ make_find_result(const char_t* const begin,
                                       const char_t* const curr) const
    {
        const size_t distance = static_cast<size_t>(curr - begin);
        const size_t used_chars = has_value_ ? distance : 0;
        return trie_find_result_{used_chars, value_};
    }

    //! Find the longest element recursively and returns it and the used length.
    trie_find_result_ find(const char_t* const original,
                           const char_t* const curr,
                           const size_t remaining) const
    {
        trie_find_result_ result;
        if (0 == remaining) {
            result = make_find_result(original, curr);
        } else {
            const trie_node* const child =
                children_[static_cast<uint8_t>(*curr)];
            if (child != nullptr) {
                result = child->find(original, curr + 1, remaining - 1);
                if (0 == result.used_size()) {
                    result = make_find_result(original, curr);
                }
            } else {
                result = make_find_result(original, curr);
            }
        }
        return result;
    }

    //! @brief Inserts a new token. Returns true, when the insertion was
    //! completely successful.
    bool insert(const char_t* token, const value_type& value,
                const bool is_case_insensitive)
    {
        bool result = false;
        if (static_cast<uint8_t>(*token) == 0) {

            // Check if node is not already possessed.
            if (!has_value_) {
                has_value_ = true;
                value_ = value;
                result = true;
            }

        } else {
            result = insert_recursive(token, value, is_case_insensitive);
        }

        return result;
    }

    //! @brief Erases an token. Returns true, when the token was completely
    //! removed.
    bool erase(const char_t* token, const bool is_case_insensitive)
    {
        bool result = false;
        if (static_cast<uint8_t>(*token) == 0) {

            // Check if node is possessed.
            if (has_value_) {
                has_value_ = false;
                result = true;
            }

        } else {
            result = erase_recursive(token, is_case_insensitive);
        }

        return result;
    }

private:
    static constexpr uint8_t LETTER_CASE_BIT = 0x20U;
    static constexpr uint8_t INVERSE_LETTER_CASE_BIT =
        static_cast<uint8_t>(~LETTER_CASE_BIT);

    void clear_right_side_when_equal(const trie_node* const left,
                                     trie_node*& right)
    {
        if (left == right) {
            right = nullptr;
        }
    }

    void add_child_reference(trie_node*& target, trie_node* const child)
    {
        target = child;
        used_children_++;
    }

    void remove_child_reference(trie_node*& child)
    {
        child = nullptr;
        used_children_--;
    }

    static uint8_t make_lower(const uint8_t c)
    {
        return c | LETTER_CASE_BIT;
    }

    static uint8_t make_upper(const uint8_t c)
    {
        return c & INVERSE_LETTER_CASE_BIT;
    }

    trie_node** get_pendant(const uint8_t c, const bool is_case_insensitive)
    {
        trie_node** result = nullptr;
        if (is_case_insensitive) {
            if (check_range<uint8_t, 'A', 'Z'>(c)) {
                result = &(children_[make_lower(c)]);
            } else if (check_range<uint8_t, 'a', 'z'>(c)) {
                result = &(children_[make_upper(c)]);
            } else {
                // Character is no letter.
            }
        }
        return result;
    }

    bool insert_recursive(const char_t* token, const value_type& value,
                          const bool is_case_insensitive)
    {
        const uint8_t c = static_cast<uint8_t>(*token);
        const char_t* next = token + 1;

        trie_node*& child = children_[c];
        if (nullptr == child) {
            add_child_reference(child, new trie_node());
        }

        bool result = false;
        if (child->insert(next, value, is_case_insensitive)) {
            trie_node** other = get_pendant(c, is_case_insensitive);
            if ((other != nullptr) && (nullptr == (*other))) {
                add_child_reference(*other, child);
            }
            result = true;
        }
        return result;
    }

    bool erase_recursive(const char_t* token, const bool is_case_insensitive)
    {
        const uint8_t c = static_cast<uint8_t>(*token);
        const char_t* next = token + 1;

        bool result = false;
        trie_node*& child = children_[c];
        if ((child != nullptr) && (child->erase(next, is_case_insensitive))) {

            if ((0 == child->used_children_) && (!child->has_value_)) {
                delete child;
                remove_child_reference(child);

                trie_node** other = get_pendant(c, is_case_insensitive);
                if ((other != nullptr) && ((*other) != nullptr)) {
                    remove_child_reference(*other);
                }
            }

            result = true;
        }
        return result;
    }

    bool has_value_;
    value_type value_;
    std::array<trie_node*, NUMBER_OF_VALUES_PER_BYTE> children_;
    size_t used_children_;
};

} // namespace detail

//! @brief Implements a trie to map a string to a specific value.
//!
//! It could be case sensitive or case insensitive and provides the basic
//! operations insert, find and erase. The find operation will search for stored
//! trie tokens at the search string's begin. It will always find the longest
//! result.
template <typename value_type>
class trie
{
public:
    //! Specializes the result of the find operation.
    using trie_find_result_ = trie_find_result<value_type>;

    static_assert(
        sizeof(uint8_t) == sizeof(char_t),
        "The trie implementation needs a char_t type that has 8 bits or"
        " it would compromise some type convertions.");

    //! @brief Determines whether the trie and all it's operations are acting
    //! case sensitive or not.
    explicit trie(const bool is_case_insensitive)
        : is_case_insensitive_(is_case_insensitive)
        , count_(new size_t())
        , root_node_(new detail::trie_node<value_type>())
    {
        assert(count_ != nullptr);
        (*count_) = 1;
    }

    //! @brief Copies a trie. This increases the number of references to the
    //! root node.
    trie(const trie& rhs)
        : is_case_insensitive_(rhs.is_case_insensitive_)
        , count_(rhs.count_)
        , root_node_(rhs.root_node_)
    {
        assert(count_ != nullptr);
        (*count_)++;
    }

    //! @brief Assigns the content of a trie. This increases the number of
    //! references to the root node.
    trie& operator=(const trie& rhs)
    {
        is_case_insensitive_ = rhs.is_case_insensitive_;
        count_ = rhs.count_;
        root_node_ = rhs.root_node_;
        assert(count_ != nullptr);
        (*count_)++;

        return *this;
    }

    //! @brief Destroys the trie. Removes the trie node, when the reference
    //! count drops to zero.
    ~trie()
    {
        (*count_)--;

        // Delete the content, when reference counter is zero.
        if (*count_ == 0) {
            delete root_node_;
            delete count_;
        }
    }

    //! @brief Returns the longest match inside the trie.
    //!
    //! At most max_length characters are read. It returns a structure of used
    //! length to get this token and result value. If no token is found, the
    //! result's used size is zero.
    trie_find_result_ find(const char_t* const search_string,
                           const size_t max_length) const
    {
        return root_node_->find(search_string, search_string, max_length);
    }

    //! @brief Inserts a token with it's value into the trie.
    //!
    //! Returns whether the token was not already in the trie and the token was
    //! therefore inserted.
    bool insert(const char_t* token, const value_type& value)
    {
        return root_node_->insert(token, value, is_case_insensitive_);
    }

    //! @brief Erases a token.
    //!
    //! Returns whether the token could be erased.
    bool erase(const char_t* token)
    {
        return root_node_->erase(token, is_case_insensitive_);
    }

private:
    //! Defines whether searching a token is done case insensitive or not.
    const bool is_case_insensitive_;

    //! Counts the number of references to the trie's root node.
    size_t* const count_;

    //! Root node of the trie as entry point for every operation.
    detail::trie_node<value_type>* root_node_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_TRIE_HPP
