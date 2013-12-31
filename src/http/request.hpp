/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

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

namespace rest
{

namespace http
{

class Request
{
public:
    explicit Request(const rest::socket::ConnectionPtr & connection);
    void parse();

    Method method() const;
    std::string url() const;
    Version version() const;
    std::string header(const std::string & key) const;
    rest::socket::Buffer data() const;

private:
    char consumeChar(size_t & index);

    rest::socket::ConnectionPtr m_connection;
    rest::socket::Buffer m_buffer;

    Method m_method;
    std::string m_url;
    Version m_version;
    std::map<std::string, std::string> m_headers;
    rest::socket::Buffer m_data;
};

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_REQUEST_HPP__
