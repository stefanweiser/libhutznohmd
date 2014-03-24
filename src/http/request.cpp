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

#include <algorithm>
#include <cctype>
#include <iterator>
#include <set>
#include <sstream>

#include "request.hpp"

namespace rest
{

namespace http
{

Request::Request(const rest::socket::ConnectionPtr & connection)
    : m_connection(connection)
    , m_buffer()
    , m_httpParser(std::bind(&Request::get, this), std::bind(&Request::peek, this))
    , m_data()
    , m_index(0)
    , m_empty()
{}

namespace
{

template <typename T>
T lexical_cast(const std::string & str)
{
    T var = T();
    std::istringstream iss;
    iss.str(str);
    iss >> var;
    return var;
}

} // namespace

void Request::parse()
{
    m_httpParser.parse();
    size_t contentLength = lexical_cast<size_t>(header("content-length"));
    while (contentLength > 0)
    {
        peek();
        size_t oldSize = m_data.size();
        m_data.insert(m_data.end(), m_buffer.begin() + m_index, m_buffer.end());
        m_index = m_buffer.size();
        contentLength -= (m_data.size() - oldSize);
    }
}

Method Request::method() const
{
    return static_cast<Method>(m_httpParser.method());
}

std::string Request::url() const
{
    return m_httpParser.url();
}

Version Request::version() const
{
    return static_cast<Version>(m_httpParser.version());
}

const std::string & Request::header(const std::string & key) const
{
    auto it = m_httpParser.headers().find(key);
    if (it != m_httpParser.headers().end())
    {
        return it->second;
    }
    return m_empty;
}

rest::Buffer Request::data() const
{
    return m_data;
}

int Request::get()
{
    if (m_index >= m_buffer.size())
    {
        if (false == m_connection->receive(m_buffer, 4000))
        {
            return '\0';
        }
    }

    return m_buffer[m_index++];
}

int Request::peek()
{
    if (m_index >= m_buffer.size())
    {
        if (false == m_connection->receive(m_buffer, 4000))
        {
            return '\0';
        }
    }

    return m_buffer[m_index];
}

} // namespace http

} // namespace rest
