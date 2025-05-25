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

#ifndef LIBHUTZNOHMD_UTILITY_TRIE_HPP
#define LIBHUTZNOHMD_UTILITY_TRIE_HPP

#include <array>
#include <cassert>

#include "libhutznohmd/types.hpp"
#include "utility/character_validation.hpp"
#include "utility/common.hpp"

namespace hutzn
{

//! @brief Result structure for each trie find operation.
//!
//! Takes a value_type from the trie it was returned from.
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

    //! @brief Constructs the result of a trie's find operation with size and
    //! value.
    //!
    //! If the used size attribute is zero, the value is undefined.
    //! @param[in] s Used size of the input string.
    //! @param[in] v Value to return.
    trie_find_result(const size_t s, const value_type& v)
        : used_size_(s)
        , value_(v)
    {
    }

    //! @brief Returns the used size attribute.
    //!
    //! @return Used size of the input string,
    size_t used_size(void) const
    {
        return used_size_;
    }

    //! @brief Returns the value attribute.
    //!
    //! If the used size is zero, this result is undefined.
    //! @return The found value.
    const value_type& value(void) const
    {
        return value_;
    }

private:
    //! Number of characters, that were used to get a valid token. Is zero if
    //! no token was found (the value is undefined then).
    size_t used_size_;

    //! Found value or undefined if no value was found.
    value_type value_;
};

namespace detail
{

//! @brief Implements a node of the trie.
//!
//! Is used for the internal representation of the trie's data tree.
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

    //! @brief Destroys the node.
    //!
    //! Deletes its children recursively. Clears the pointers if they are case
    //! sensitive.
    ~trie_node(void) noexcept(true)
    {
        // release all children first
        for (size_t i = 0; i < children_.size(); i++) {
            const uint8_t c = static_cast<uint8_t>(i);
            trie_node* child = children_[c];

            // delete only if set
            if (child != NULL) {
                delete child;

                // when the corresponding lower case points to the same data,
                // just clear it, because it got already deleted
                if (check_range<uint8_t, 'A', 'Z'>(c)) {
                    trie_node*& other = children_[make_lower(c)];
                    clear_right_side_when_equal(child, other);
                }
            }
        }
    }

    //! @brief Makes a find result.
    //!
    //! This is done by calculating the distance between curr and begin pointer.
    //! @param[in] begin Points to the begin of the search area.
    //! @param[in] curr  Points to the current character (and is therefore the
    //!                  end of the found value).
    //! @return          A trie_find_result.
    trie_find_result_ make_find_result(const char_t* const begin,
                                       const char_t* const curr) const
    {
        const size_t distance = static_cast<size_t>(curr - begin);
        const size_t used_chars = has_value_ ? distance : 0;
        return trie_find_result_{used_chars, value_};
    }

    //! @brief Searches the longest element recursively.
    //!
    //! Returns it and the used length, if found. Stops searching at the end of
    //! the string, which is defined by the remaining parameter.
    //! @param[in] original  Points to the begin of the search area.
    //! @param[in] curr      Points to the current character.
    //! @param[in] remaining Stores the remaining number of characters before
    //!                      the string's end is reached.
    //! @return              A trie_find_result.
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
            if (child != NULL) {
                result = child->find(original, curr + 1, remaining - 1);
                // if there is no longer match, take this result
                if (0 == result.used_size()) {
                    result = make_find_result(original, curr);
                }
            } else {
                // found a matching result
                result = make_find_result(original, curr);
            }
        }
        return result;
    }

    //! @brief Inserts a value.
    //!
    //! Returns true, when the insertion was completely successful.
    //! @param[in] token               Token string by which the value will be
    //!                                found.
    //! @param[in] value               Value, that gets inserted.
    //! @param[in] is_case_insensitive If true, it will insert the token case
    //!                                insensitive.
    //! @return                        True, when the value was inserted
    //!                                successfully or false if not.
    bool insert(const char_t* token, const value_type& value,
                const bool is_case_insensitive)
    {
        bool result = false;
        if ((*token) == '\0') {

            // check if node is not already possessed
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

    //! @brief Erases a value.
    //!
    //! Returns true, when the value was removed completely.
    //! @param[in] token               Token string which gets erased.
    //! @param[in] is_case_insensitive If true, it will erase the token case
    //!                                insensitive.
    //! @return                        True, when the value was removed
    //!                                successfully or false if not.
    bool erase(const char_t* token, const bool is_case_insensitive)
    {
        bool result = false;
        if ((*token) == '\0') {

            // check if node is possessed
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
    //! @brief Clears the right operand to NULL, when both pointers are equal.
    //!
    //! @param[in]     left  Left operand.
    //! @param[in,out] right Right operand.
    void clear_right_side_when_equal(const trie_node* const left,
                                     trie_node*& right)
    {
        if (left == right) {
            right = NULL;
        }
    }

    //! @brief Adds a child reference to reference counters and sets the target
    //! trie node.
    //!
    //! @param[out] target Gets set to the child parameter.
    //! @param[in]  child  Child parameter.
    void add_child_reference(trie_node*& target, trie_node* const child)
    {
        target = child;
        used_children_++;
    }

    //! @brief Removes a child reference and resets the child parameter.
    //!
    //! @param[in,out] child Child parameter.
    void remove_child_reference(trie_node*& child)
    {
        child = NULL;
        used_children_--;
    }

    //! @brief Converts an upper case character to lower case.
    //!
    //! @param[in] c Character to convert.
    //! @return      Lower case representation of the upper case letter stored
    //!              in c.
    static uint8_t make_lower(const uint8_t c)
    {
        return c | letter_case_offset;
    }

    //! @brief Converts an lower case character to upper case.
    //!
    //! @param[in] c Character to convert.
    //! @return      Upper case representation of the lower case letter stored
    //!              in c.
    static uint8_t make_upper(const uint8_t c)
    {
        return c & static_cast<uint8_t>(~letter_case_offset);
    }

    //! @brief Determines the corresponding tried_node of the case sensitivity.
    //!
    //! @param[in] c                   Character to check.
    //! @param[in] is_case_insensitive Pendant may be available when this is
    //!                                true.
    //! @return                        A pointer which points to the
    //!                                corresponding trie_node pointer.
    trie_node** get_pendant(const uint8_t c, const bool is_case_insensitive)
    {
        trie_node** result = NULL;
        if (is_case_insensitive) {
            if (check_range<uint8_t, 'A', 'Z'>(c)) {
                // upper case
                result = &(children_[make_lower(c)]);
            } else if (check_range<uint8_t, 'a', 'z'>(c)) {
                // lower case
                result = &(children_[make_upper(c)]);
            } else {
                // character is no letter
            }
        }
        return result;
    }

    //! @brief Inserts a value recursively.
    //!
    //! @param[in] token               Token string by which the value will be
    //!                                found.
    //! @param[in] value               Value, that gets inserted.
    //! @param[in] is_case_insensitive If true, it will insert the token case
    //!                                insensitive.
    //! @return                        True, when the value was inserted
    //!                                successfully or false if not.
    bool insert_recursive(const char_t* token, const value_type& value,
                          const bool is_case_insensitive)
    {
        const uint8_t c = static_cast<uint8_t>(*token);
        const char_t* next = token + 1;

        // add usage reference
        trie_node*& child = children_[c];
        if (NULL == child) {
            add_child_reference(child, new trie_node());
        }

        bool result = false;
        if (child->insert(next, value, is_case_insensitive)) {
            // add a reference also to the pendant trie node
            trie_node** other = get_pendant(c, is_case_insensitive);
            if ((other != NULL) && (NULL == (*other))) {
                add_child_reference(*other, child);
            }
            result = true;
        }
        return result;
    }

    //! @brief Erases a value recursively.
    //!
    //! Returns true, when the value was removed completely.
    //! @param[in] token               Token string which gets erased.
    //! @param[in] is_case_insensitive If true, it will erase the token case
    //!                                insensitive.
    //! @return                        True, when the value was removed
    //!                                successfully or false if not.
    bool erase_recursive(const char_t* token, const bool is_case_insensitive)
    {
        const uint8_t c = static_cast<uint8_t>(*token);
        const char_t* next = token + 1;

        bool result = false;
        trie_node*& child = children_[c];

        if ((child != NULL) && (child->erase(next, is_case_insensitive))) {

            if ((0 == child->used_children_) && (!child->has_value_)) {

                // delete the child and reduce the number of used children
                delete child;
                remove_child_reference(child);

                // in case of case insensitivity, reduce the number of used
                // children of the pendant trie node also
                trie_node** other = get_pendant(c, is_case_insensitive);
                if ((other != NULL) && ((*other) != NULL)) {
                    remove_child_reference(*other);
                }
            }

            result = true;
        }
        return result;
    }

    //! True, when the member value_ has a defined meaning.
    bool has_value_;

    //! Value stored in that node. Is only of interest, when has_value_ is true.
    value_type value_;

    //! Contains all children. The key is the character, that is read from the
    //! input string.
    std::array<trie_node*, byte_state_count> children_;

    //! Reference counter, which counts the number of used children.
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
        "the trie implementation needs a char_t type that has 8 bits or it "
        "would compromise some type convertions");

    //! @brief Constructs the trie.
    //!
    //! @param[in] is_case_insensitive Determines whether the trie and all it's
    //!                                operations are acting case sensitive or
    //!                                not.
    explicit trie(const bool is_case_insensitive)
        : is_case_insensitive_(is_case_insensitive)
        , count_(new size_t())
        , root_node_(new detail::trie_node<value_type>())
    {
        assert(count_ != NULL);
        (*count_) = 1;
    }

    //! @brief Copies a trie.
    //!
    //! This increases the number of references to the root node.
    //! @param[in] rhs Object to copy.
    trie(const trie& rhs)
        : is_case_insensitive_(rhs.is_case_insensitive_)
        , count_(rhs.count_)
        , root_node_(rhs.root_node_)
    {
        assert(count_ != NULL);

        // increases the counter
        (*count_)++;
    }

    //! @brief Assigns the content of a trie.
    //!
    //! This increases the number of references to the root node.
    //! @param[in] rhs Object to copy.
    //! @return        Assigned object.
    trie& operator=(const trie& rhs)
    {
        is_case_insensitive_ = rhs.is_case_insensitive_;

        // copies the pointer and the counter
        count_ = rhs.count_;
        root_node_ = rhs.root_node_;
        assert(count_ != NULL);

        // increases the counter
        (*count_)++;

        return *this;
    }

    //! @brief Destroys the trie.
    //!
    //! Deletes the trie node, when the reference count drops to zero.
    ~trie(void) noexcept(true)
    {
        // decrease the reference counter
        (*count_)--;

        // delete the content, when reference counter is zero
        if (*count_ == 0) {
            // delete the counter and the data structure as well
            delete root_node_;
            delete count_;
        }
    }

    //! @brief Returns the longest match inside the trie.
    //!
    //! At most max_length characters are read. It returns a structure of used
    //! length to get this token and result value. If no token is found, the
    //! result's used size is zero.
    //! @param[in] search_string String to find a token on.
    //! @param[in] max_length    Maximum search depth, due to the string length.
    //! @return                  A trie_find_result.
    trie_find_result_ find(const char_t* const search_string,
                           const size_t max_length) const
    {
        return root_node_->find(search_string, search_string, max_length);
    }

    //! @brief Inserts a token with it's value into the trie.
    //!
    //! Returns whether the token was not already in the trie and the token was
    //! therefore inserted.
    //! @param[in] token String that will result in the value parameter.
    //! @param[in] value Value to insert.
    //! @return          True if the insertion was successful and false when
    //!                  not.
    bool insert(const char_t* token, const value_type& value)
    {
        return root_node_->insert(token, value, is_case_insensitive_);
    }

    //! @brief Erases a token.
    //!
    //! Returns whether the token could be erased.
    //! @param[in] token String to remove.
    //! @return          True if removing was successful and false when not.
    bool erase(const char_t* token)
    {
        return root_node_->erase(token, is_case_insensitive_);
    }

private:
    //! Defines whether searching a token is done case insensitive or not.
    bool is_case_insensitive_;

    //! Counts the number of references to the trie's root node.
    size_t* count_;

    //! Root node of the trie as entry point for every operation.
    detail::trie_node<value_type>* root_node_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_TRIE_HPP
