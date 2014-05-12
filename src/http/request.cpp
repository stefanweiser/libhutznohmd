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

#include <algorithm>
#include <cctype>
#include <iterator>
#include <set>

#include "request.hpp"

namespace rest
{

namespace http
{

int32_t get_char(void * handle)
{
    return static_cast<request *>(handle)->get();
}

int32_t peek_char(void * handle)
{
    return static_cast<request *>(handle)->peek();
}

request::request(const rest::socket::connection_pointer & connection)
    : connection_(connection)
    , buffer_()
    , request_parser_(anonymous_int_function(&get_char, this),
                      anonymous_int_function(&peek_char, this))
    , data_()
    , index_(0)
{}

void request::parse()
{
    request_parser_.parse();
    ssize_t content_length = request_parser_.content_length();

    while ((content_length > 0) && (peek() >= 0)) {
        ssize_t old_size = data_.size();
        ssize_t old_buffer_size = static_cast<ssize_t>(buffer_.size());
        if ((old_buffer_size - static_cast<ssize_t>(index_)) >= content_length) {
            data_.insert(data_.end(),
                         buffer_.begin() + index_,
                         buffer_.begin() + index_ + content_length);
            index_ += content_length;
            content_length = 0;
        } else {
            data_.insert(data_.end(), buffer_.begin() + index_, buffer_.end());
            index_ = buffer_.size();
            content_length -= (data_.size() - old_size);
        }
    }
}

rest::http::method request::method() const
{
    return static_cast<rest::http::method>(request_parser_.method());
}

std::string request::url() const
{
    return request_parser_.url();
}

rest::http::version request::version() const
{
    return static_cast<rest::http::version>(request_parser_.version());
}

const std::string request::header(const std::string & key) const
{
    return request_parser_.header(key);
}

rest::buffer request::data() const
{
    return data_;
}

time_t request::date() const
{
    return request_parser_.date();
}

bool request::is_keep_connection() const
{
    return request_parser_.is_keep_connection();
}

int32_t request::get()
{
    if (index_ >= buffer_.size()) {
        if (false == connection_->receive(buffer_, 4000)) {
            return -1;
        }
    }

    return buffer_[index_++];
}

int32_t request::peek()
{
    if (index_ >= buffer_.size()) {
        if (false == connection_->receive(buffer_, 4000)) {
            return -1;
        }
    }

    return buffer_[index_];
}

} // namespace http

} // namespace rest
