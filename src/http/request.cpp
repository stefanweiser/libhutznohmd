/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <cctype>
#include <iterator>
#include <set>

#include <http/parser/utility/md5.hpp>

#include "request.hpp"

namespace rest
{

namespace http
{

int32_t get_char(void* handle) { return static_cast<request*>(handle)->get(); }

int32_t peek_char(void* handle)
{
    return static_cast<request*>(handle)->peek();
}

request::request(const rest::socket::connection_pointer& connection,
                 const parameters& param)
    : connection_(connection)
    , buffer_()
    , request_parser_(anonymous_int_function(&get_char, this),
                      anonymous_int_function(&peek_char, this))
    , data_()
    , index_(0)
    , parameters_(param)
{
}

bool request::parse()
{
    request_parser_.parse();
    ssize_t content_length = request_parser_.content_length();

    while ((content_length > 0) && (peek() >= 0)) {
        ssize_t old_size = data_.size();
        ssize_t old_buffer_size = static_cast<ssize_t>(buffer_.size());
        if ((old_buffer_size - static_cast<ssize_t>(index_)) >=
            content_length) {
            data_.insert(data_.end(), buffer_.begin() + index_,
                         buffer_.begin() + index_ + content_length);
            index_ += content_length;
            content_length = 0;
        } else {
            data_.insert(data_.end(), buffer_.begin() + index_, buffer_.end());
            index_ = buffer_.size();
            content_length -= (data_.size() - old_size);
        }
    }

    bool result = request_parser_.valid();
    if ((true == result) && (true == request_parser_.has_md5()) &&
        (true == parameters_.check_md5)) {
        const std::array<uint8_t, 16>& md5 = request_parser_.md5();
        result = (md5 == calculate_md5(data_));
    }
    return result;
}

rest::http::method request::method() const
{
    return static_cast<rest::http::method>(request_parser_.method());
}

const uri_interface& request::request_uri() const
{
    return request_parser_.request_uri();
}

rest::http::version request::version() const
{
    return static_cast<rest::http::version>(request_parser_.version());
}

const std::map<std::string, std::string>& request::headers() const
{
    return request_parser_.headers();
}

const rest::buffer& request::data() const { return data_; }

const media_type_interface& request::data_content_type() const
{
    return request_parser_.content_type();
}

time_t request::date() const { return request_parser_.date(); }

bool request::keeps_connection() const
{
    return request_parser_.keeps_connection();
}

int32_t request::get()
{
    if (index_ >= buffer_.size()) {
        if (false == connection_->receive(buffer_, 4000)) {
            return -1;
        }
    }

    return static_cast<uint8_t>(buffer_[index_++]);
}

int32_t request::peek()
{
    if (index_ >= buffer_.size()) {
        if (false == connection_->receive(buffer_, 4000)) {
            return -1;
        }
    }

    return static_cast<uint8_t>(buffer_[index_]);
}

} // namespace http

} // namespace rest
