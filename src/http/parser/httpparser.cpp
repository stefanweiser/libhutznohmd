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
#include <limits>
#include <map>

#include "httpparser.hpp"

extern "C"
{

    int32_t httpparse(httpscan_t * scanner);

}

namespace rest
{

namespace http
{

http_parser::http_parser(const anonymous_int_function & get_functor,
                         const anonymous_int_function & peek_functor)
    : httpscan_({lexer(get_functor, peek_functor), parser_state::UNFINISHED, push_back_string<40>(),
    push_back_string<1000>(), rest::http::method::UNKNOWN, rest::http::version::HTTP_UNKNOWN,
    push_back_string<1000>(), 0, push_back_string<100>(), std::map<std::string, std::string>(), 0,
    time(NULL)
})
{}

void http_parser::parse()
{
    if (parser_state::UNFINISHED == httpscan_.state_) {
        http_parse(&httpscan_);
    }
}

bool http_parser::valid() const
{
    return (parser_state::SUCCEEDED == httpscan_.state_);
}

const rest::http::method & http_parser::method() const
{
    return httpscan_.method_;
}

const rest::http::version & http_parser::version() const
{
    return httpscan_.version_;
}

const std::string http_parser::url() const
{
    return std::string(httpscan_.url_.c_str());
}

const uint16_t & http_parser::status_code() const
{
    return httpscan_.status_code_;
}

const std::string http_parser::reason_phrase() const
{
    return std::string(httpscan_.reason_phrase_.c_str());
}

const std::string http_parser::header(const std::string & key) const
{
    auto it = httpscan_.headers_.find(key);
    if (it != httpscan_.headers_.end()) {
        return it->second;
    }
    return "";
}

const size_t & http_parser::content_length() const
{
    return httpscan_.content_length_;
}

const time_t & http_parser::date() const
{
    return httpscan_.date_;
}

} // namespace http

} // namespace rest
