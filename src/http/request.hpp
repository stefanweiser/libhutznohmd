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

#ifndef __LIBREST_HTTP_REQUEST_HPP__
#define __LIBREST_HTTP_REQUEST_HPP__

#include <ctime>
#include <map>
#include <memory>

#include <socket/listenersocket.hpp>

#include <librest.hpp>

#include <http/parser/httpparser.hpp>

namespace rest
{

namespace http
{

class request: public request_interface
{
public:
    explicit request(const rest::socket::connection_pointer & connection);
    void parse();

    virtual rest::http::method method() const;
    virtual std::string url() const;
    virtual rest::http::version version() const;
    virtual const std::string header(const header_type & type) const;
    virtual const std::string custom_header(const std::string & key) const;
    virtual rest::buffer data() const;
    virtual time_t date();

private:
    int get();
    int peek();

    rest::socket::connection_pointer connection_;
    rest::buffer buffer_;
    rest::http::http_parser http_parser_;
    rest::buffer data_;
    size_t index_;
    time_t date_;
};

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_REQUEST_HPP__
