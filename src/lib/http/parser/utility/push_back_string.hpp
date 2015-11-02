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

#ifndef LIBHUTZNOHMD_HTTP_PARSER_UTILITY_PUSH_BACK_STRING_HPP
#define LIBHUTZNOHMD_HTTP_PARSER_UTILITY_PUSH_BACK_STRING_HPP

#include <cstring>
#include <cstdlib>

#include <libhutznohmd/types.hpp>

namespace hutzn
{

namespace http
{

//! Provides a tradeoff between fast and static but limited string types and
//! slower and dynamic string types. Its maximum_size at begin is predefined
//! by a template argument. The user is only able to push one character at the
//! back of the string or clear the string at all to manipulate the string.
template <size_t maximum_size>
class push_back_string
{
public:
    //! Constructs a new and empty string without any dynamic memory allocation.
    explicit push_back_string();

    //! If any dynamic memory was allocated, this memory get freed here.
    ~push_back_string();

    //! Pushs one character at the end of the string. The string won't get
    //! finished by '\0'. It allocates memory dynamically if the string runs
    //! out of memory.
    void push_back(const char_t c);

    //! Pushs some character at the end of the string. The string won't get
    //! finished by '\0'. It allocates memory dynamically if the string runs
    //! out of memory.
    //! WARNING: This method may never return, if the argument is not finished
    //! by '\0'. Do not use it for user defined data. This may cause a buffer
    //! overflow.
    void append_string(const char_t* s);

    //! Returns the character at a given index.
    char_t& operator[](const size_t& index);

    //! Returns the current string. Before returning it, the data will be
    // terminated by '\0'.
    const char_t* c_str() const;

    //! Returns true if the current length is 0.
    bool empty() const;

    //! Returns the current size of the string.
    size_t size() const;

    //! Clears the string. Frees the dynamically allocated memory if needed.
    void clear();

    push_back_string(const push_back_string&) = delete;
    push_back_string& operator=(const push_back_string&) = delete;

private:
    void need_more_dynamic_memory();

    size_t current_length_;
    size_t dynamic_size_;
    mutable char_t static_buffer_[maximum_size + 1];
    mutable char_t* dynamic_buffer_;
};

//! ...
template <size_t maximum_size>
push_back_string<maximum_size>::push_back_string()
    : current_length_(0)
    , dynamic_size_(0)
    , static_buffer_()
    , dynamic_buffer_(NULL)
{
}

//! ...
template <size_t maximum_size>
push_back_string<maximum_size>::~push_back_string()
{
    free(dynamic_buffer_);
}

//! ...
template <size_t maximum_size>
void push_back_string<maximum_size>::push_back(const char_t c)
{
    // It makes some difference to push it onto the static or the dynamically
    // allocated buffer.
    if (current_length_ < maximum_size) {
        static_buffer_[current_length_++] = c;
    } else {
        // The string is too long for static memory.
        if ((current_length_ + 1) >= dynamic_size_) {
            need_more_dynamic_memory();
        }
        dynamic_buffer_[current_length_++] = c;
    }
}

//! ...
template <size_t maximum_size>
void push_back_string<maximum_size>::append_string(const char_t* s)
{
    while ((*s) != '\0') {
        push_back(*s);
        s++;
    }
}

//! ...
template <size_t maximum_size>
char_t& push_back_string<maximum_size>::operator[](const size_t& index)
{
    if (NULL == dynamic_buffer_) {
        return static_buffer_[index];
    } else {
        return dynamic_buffer_[index];
    }
}

//! ...
template <size_t maximum_size>
const char_t* push_back_string<maximum_size>::c_str() const
{
    if (NULL == dynamic_buffer_) {
        // Terminate the data before returning it.
        static_buffer_[current_length_] = '\0';
        return static_buffer_;
    } else {
        // Terminate the data before returning it.
        dynamic_buffer_[current_length_] = '\0';
        return dynamic_buffer_;
    }
}

//! ...
template <size_t maximum_size>
bool push_back_string<maximum_size>::empty() const
{
    return current_length_ == 0;
}

//! ...
template <size_t maximum_size>
size_t push_back_string<maximum_size>::size() const
{
    return current_length_;
}

//! ...
template <size_t maximum_size>
void push_back_string<maximum_size>::clear()
{
    if (NULL != dynamic_buffer_) {
        // We need to free the buffer, because we will work with the static data
        // again.
        free(dynamic_buffer_);
        dynamic_buffer_ = NULL;
        dynamic_size_ = 0;
    }
    current_length_ = 0;
}

//! ...
template <size_t maximum_size>
void push_back_string<maximum_size>::need_more_dynamic_memory()
{
    if (NULL == dynamic_buffer_) {
        dynamic_size_ = (maximum_size + maximum_size) + 1;
        dynamic_buffer_ = static_cast<char_t*>(malloc(dynamic_size_));
        memcpy(dynamic_buffer_, static_buffer_, maximum_size);
    } else {
        dynamic_size_ += maximum_size;

        // Don't forget to free the buffer block that was used before
        // reallocation in case of a failed reallocation.
        char_t* new_buffer =
            static_cast<char_t*>(realloc(dynamic_buffer_, dynamic_size_));
        if (NULL == new_buffer) {
            free(dynamic_buffer_);
        }
        dynamic_buffer_ = new_buffer;
    }
}

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HTTP_PARSER_UTILITY_PUSH_BACK_STRING_HPP
