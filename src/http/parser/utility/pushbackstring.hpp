/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBREST_HTTP_PARSER_UTILITY_PUSHBACKSTRING_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_PUSHBACKSTRING_HPP__

#include <cstring>

namespace rest
{

namespace http
{

//! Provides a tradeoff between fast and static but limited string types and slower and dynamic
//! string types. Its size at begin is predefined by a template argument. The user is only able
//! to push one character at the back of the string or clear the string at all to manipulate the
//! string.
template<size_t size>
class push_back_string
{
public:
    //! Constructs a new and empty string without any dynamic memory allocation.
    explicit push_back_string();

    //! If any dynamic memory was allocated, this memory get freed here.
    ~push_back_string();

    push_back_string(const push_back_string &);
    push_back_string & operator=(const push_back_string &) = delete;

    //! Pushs one character at the end of the string. The string won't get finished by '\0'. It
    //! allocates memory dynamically if the string runs out of memory.
    void push_back(const char c);

    //! Returns the current string. Before returning it, the data will be terminated by '\0'.
    const char * c_str() const;

    //! Returns true if the current length is 0.
    bool empty() const;

    //! Clears the string. Frees the dynamically allocated memory if needed.
    void clear();

private:
    size_t current_length_;
    size_t dynamic_size_;
    mutable char static_buffer_[size + 1];
    mutable char * dynamic_buffer_;
};

template<size_t size>
push_back_string<size>::push_back_string()
    : current_length_(0)
    , dynamic_size_(0)
    , static_buffer_()
    , dynamic_buffer_(nullptr)
{
}

template<size_t size>
push_back_string<size>::~push_back_string()
{
    free(dynamic_buffer_);
}

template<size_t size>
push_back_string<size>::push_back_string(const push_back_string &)
    : current_length_(0)
    , dynamic_size_(0)
    , static_buffer_()
    , dynamic_buffer_(nullptr)
{
}

template<size_t size>
void push_back_string<size>::push_back(const char c)
{
    // It makes some difference to push it onto the static or the dynamically allocated buffer.
    if (current_length_ < size) {
        static_buffer_[current_length_++] = c;
    } else {
        // The string is too long for static memory.
        if ((current_length_ + 1) >= dynamic_size_) {
            if (nullptr == dynamic_buffer_) {
                dynamic_size_ = (2 * size) + 1;
                dynamic_buffer_ = static_cast<char *>(malloc(dynamic_size_));
                memcpy(dynamic_buffer_, static_buffer_, size);
            } else {
                dynamic_size_ += size;

                // Don't forget to free the buffer block that was used before reallocation in case
                // of a failed reallocation.
                char * new_buffer = static_cast<char *>(realloc(dynamic_buffer_, dynamic_size_));
                if (nullptr == new_buffer) {
                    free(dynamic_buffer_);
                }
                dynamic_buffer_ = new_buffer;
            }
        }
        dynamic_buffer_[current_length_++] = c;
    }
}

template<size_t size>
const char * push_back_string<size>::c_str() const
{
    if (nullptr == dynamic_buffer_) {
        // Terminate the data before returning it.
        static_buffer_[current_length_] = '\0';
        return static_buffer_;
    } else {
        // Terminate the data before returning it.
        dynamic_buffer_[current_length_] = '\0';
        return dynamic_buffer_;
    }
}

template<size_t size>
bool push_back_string<size>::empty() const
{
    return (current_length_ == 0);
}

template<size_t size>
void push_back_string<size>::clear()
{
    if (nullptr != dynamic_buffer_) {
        // We need to free the buffer, because we will work with the static data again.
        free(dynamic_buffer_);
        dynamic_size_ = 0;
    }
    current_length_ = 0;
}

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_PUSHBACKSTRING_HPP__
