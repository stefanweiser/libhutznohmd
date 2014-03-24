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

#include <map>
#include <memory>

#include <socket/listenersocket.hpp>

#include <librest.hpp>

#include <http/parser/lexer.h>

namespace rest
{

namespace http
{

class Request: public RequestInterface
{
public:
    explicit Request(const rest::socket::ConnectionPtr & connection);
    void parse();

    virtual Method method() const;
    virtual std::string url() const;
    virtual Version version() const;
    virtual const std::string & header(const std::string & key) const;
    virtual rest::Buffer data() const;

private:
    int get();
    int peek();

    rest::socket::ConnectionPtr m_connection;
    rest::Buffer m_buffer;
    rest::http::HttpParser m_httpParser;
    rest::Buffer m_data;
    size_t m_index;
    std::string m_empty;
};

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_REQUEST_HPP__
