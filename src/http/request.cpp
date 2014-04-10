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

#include <algorithm>
#include <cctype>
#include <iterator>
#include <set>

#include <http/parser/header/httpdateparser.h>

#include "request.hpp"

namespace rest
{

namespace http
{

int anonymous_get(void * handle)
{
    return static_cast<request *>(handle)->get();
}

int anonymous_peek(void * handle)
{
    return static_cast<request *>(handle)->peek();
}

request::request(const rest::socket::connection_pointer & connection)
    : connection_(connection)
    , buffer_()
    , http_parser_(anonymous_int_function(&anonymous_get, this),
                   anonymous_int_function(&anonymous_peek, this))
    , data_()
    , index_(0)
    , date_(0)
{}

void request::parse()
{
    http_parser_.parse();
    ssize_t content_length = http_parser_.content_length();
    while ((content_length > 0) && (peek() >= 0)) {
        size_t old_size = data_.size();
        data_.insert(data_.end(), buffer_.begin() + index_, buffer_.end());
        index_ = buffer_.size();
        content_length -= (data_.size() - old_size);
    }
}

rest::http::method request::method() const
{
    return static_cast<rest::http::method>(http_parser_.method());
}

std::string request::url() const
{
    return http_parser_.url();
}

rest::http::version request::version() const
{
    return static_cast<rest::http::version>(http_parser_.version());
}

const std::string request::custom_header(const std::string & key) const
{
    return http_parser_.custom_header(key);
}

rest::buffer request::data() const
{
    return data_;
}

time_t request::date()
{
    if (date_ == 0) {
        const std::string & date_string = custom_header("date");
        http_date_parser parser(date_string);
        if (true == parser.valid()) {
            date_ = parser.timestamp();
        } else {
            date_ = time(NULL);
        }
    }
    return date_;
}

int request::get()
{
    if (index_ >= buffer_.size()) {
        if (false == connection_->receive(buffer_, 4000)) {
            return -1;
        }
    }

    return buffer_[index_++];
}

int request::peek()
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
