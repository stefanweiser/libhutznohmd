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

#include <cmath>
#include <cstdint>
#include <map>

#include "httpparser.hpp"

extern "C"
{

    int httpparse(httpscan_t * scanner);

}

typedef struct httpscan {
    rest::http::lexer * m_lexer;
    rest::http::data * m_data;
} httpscan_t;

namespace rest
{

namespace http
{

http_parser::http_parser(const std::function<int()> & get_functor,
                         const std::function<int()> & peek_functor)
    : lexer_(get_functor, peek_functor)
    , data_()
    , httpscan_(new httpscan {&lexer_, &data_})
{}

http_parser::~http_parser()
{
    delete httpscan_;
}

void http_parser::parse()
{
    if (false == lexer_.finished()) {
        httpparse(httpscan_);
    }
}

bool http_parser::valid() const
{
    return lexer_.finished();
}

const http_method & http_parser::method() const
{
    return data_.method();
}

const http_version & http_parser::version() const
{
    return data_.version();
}

const std::string http_parser::url() const
{
    return data_.url();
}

const uint16_t & http_parser::status_code() const
{
    return data_.status_code();
}

const std::string http_parser::reason_phrase() const
{
    return data_.reason_phrase();
}

const std::map<std::string, std::string> & http_parser::headers() const
{
    return data_.headers();
}

const size_t & http_parser::content_length() const
{
    return data_.content_length();
}

} // namespace http

} // namespace rest
