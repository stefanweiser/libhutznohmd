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

namespace rest
{

namespace http
{

http_parser::http_parser(const std::function<int()> & get_functor,
                         const std::function<int()> & peek_functor)
    : httpscan_({get_functor, peek_functor, 0, false, "", "", "", METHOD_UNKNOWN, VERSION_UNKNOWN,
    "", 0, "", std::map<rest::http::header_type, std::string>(),
    std::map<std::string, std::string>(), 0
})
{}

void http_parser::parse()
{
    if (false == httpscan_.finished_) {
        httpparse(&httpscan_);
    }
}

bool http_parser::valid() const
{
    return httpscan_.finished_;
}

const http_method & http_parser::method() const
{
    return httpscan_.method_;
}

const http_version & http_parser::version() const
{
    return httpscan_.version_;
}

const std::string http_parser::url() const
{
    return httpscan_.url_;
}

const uint16_t & http_parser::status_code() const
{
    return httpscan_.status_code_;
}

const std::string http_parser::reason_phrase() const
{
    return httpscan_.reason_phrase_;
}

const std::string & http_parser::header(const header_type & type) const
{
    auto it = httpscan_.headers_.find(type);
    if (it != httpscan_.headers_.end()) {
        return it->second;
    }
    return httpscan_.empty_;
}

const std::string & http_parser::custom_header(const std::string & key) const
{
    auto it = httpscan_.custom_headers_.find(key);
    if (it != httpscan_.custom_headers_.end()) {
        return it->second;
    }
    return httpscan_.empty_;
}

const size_t & http_parser::content_length() const
{
    return httpscan_.content_length_;
}

} // namespace http

} // namespace rest
