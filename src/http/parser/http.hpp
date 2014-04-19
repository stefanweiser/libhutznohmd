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

#ifndef __LIBREST_HTTP_PARSER_HTTP_HPP__
#define __LIBREST_HTTP_PARSER_HTTP_HPP__

#include <cstring>
#include <functional>
#include <map>
#include <string>

#include <librest.hpp>

#include <http/parser/utility/lexer.hpp>

enum class lexer_state
{
    UNFINISHED = 0,
    FINISHED = 1,
    ERROR = 2
};

template<size_t size>
class push_back_string
{
public:
    explicit push_back_string();
    ~push_back_string();
    push_back_string(const push_back_string &);
    push_back_string & operator=(const push_back_string &) = delete;
    void push_back(const char c);
    const char * c_str() const;
    bool empty() const;
    void clear();

private:
    size_t current_length_;
    size_t dynamic_size_;
    mutable char constant_buffer_[size + 1];
    mutable char * dynamic_buffer_;
};

template<size_t size>
push_back_string<size>::push_back_string()
    : current_length_(0)
    , dynamic_size_(0)
    , constant_buffer_()
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
    , constant_buffer_()
    , dynamic_buffer_(nullptr)
{
}

template<size_t size>
void push_back_string<size>::push_back(const char c)
{
    if (current_length_ < size) {
        constant_buffer_[current_length_++] = c;
    } else {
        if ((current_length_ + 1) >= dynamic_size_) {
            if (nullptr == dynamic_buffer_) {
                dynamic_size_ = (2 * size) + 1;
                dynamic_buffer_ = static_cast<char *>(malloc(dynamic_size_));
                memcpy(dynamic_buffer_, constant_buffer_, size);
            } else {
                dynamic_size_ += size;
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
        constant_buffer_[current_length_] = '\0';
        return constant_buffer_;
    } else {
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
        free(dynamic_buffer_);
        dynamic_size_ = 0;
    }
    current_length_ = 0;
}

typedef struct httpscan {
    lexer lexer_;
    lexer_state state_;

    push_back_string<40> header_key_;
    push_back_string<1000> header_value_;
    rest::http::method method_;
    rest::http::version version_;
    push_back_string<1000> url_;
    uint16_t status_code_;
    push_back_string<100> reason_phrase_;
    std::map<std::string, std::string> headers_;
    size_t content_length_;
    time_t date_;
} httpscan_t;

time_t parse_timestamp(int & character, httpscan_t * scanner);
void http_parse(httpscan_t * scanner);
rest::http::header_type header_key_to_header_type(const std::string & s);

#endif // __LIBREST_HTTP_PARSER_HTTP_HPP__
