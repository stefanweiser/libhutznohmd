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

#include <cctype>
#include <iostream>
#include <iomanip>
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
    , m_method(Method::UNKNOWN)
    , m_url()
    , m_version(Version::HTTP_UNKNOWN)
    , m_headers()
    , m_data()
{}

void Request::parse()
{
    bool emptyLine;
    std::vector<std::pair<size_t, size_t>> lineIndices;
    size_t dataBegin;
    {
        size_t i = 0;
        do
        {
            emptyLine = true;
            const size_t begin = i;
            char c = consumeChar(i);
            while (c != '\n')
            {
                emptyLine = false;
                c = consumeChar(i);
            }
            lineIndices.push_back(std::make_pair(begin, i));
        }
        while (false == emptyLine);
        dataBegin = i;
    }

    for ( size_t i = 0; i < lineIndices.size(); i++ )
    {
        const auto& pair = lineIndices[i];
        if ( i == 0 )
        {
            // Request-Head
            std::vector<std::string> words;
            std::string head;
            for ( size_t j = pair.first; j < pair.second; j++ )
            {
                head += m_buffer[j];
            }
            std::istringstream iss(head);
            std::copy(std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>(),
                std::back_inserter<std::vector<std::string>>(words));
            if ( words.size() != 3 )
            {
                throw std::exception();
            }

            if ( words[0] == "OPTIONS" )
            {
                m_method = Method::OPTIONS;
            }
            else if ( words[0] == "GET" )
            {
                m_method = Method::GET;
            }
            else if ( words[0] == "HEAD" )
            {
                m_method = Method::HEAD;
            }
            else if ( words[0] == "POST" )
            {
                m_method = Method::POST;
            }
            else if ( words[0] == "PUT" )
            {
                m_method = Method::PUT;
            }
            else if ( words[0] == "DELETE" )
            {
                m_method = Method::DELETE;
            }
            else if ( words[0] == "TRACE" )
            {
                m_method = Method::TRACE;
            }
            else if ( words[0] == "CONNECT" )
            {
                m_method = Method::CONNECT;
            }
            else
            {
                throw std::exception();
            }

            m_url = words[1];

            if ( words[2] == "HTTP/1.0" )
            {
                m_version = Version::HTTP_1_0;
            }
            else if ( words[2] == "HTTP/1.1" )
            {
                m_version = Version::HTTP_1_1;
            }
            else
            {
                throw std::exception();
            }
        }
        else
        {
            // Reqeust body
            std::string key;
            std::string value;
            std::set<char> whitespace = { '\t', ' ', '\n', 'r' };
            bool isKey = true;
            for ( size_t j = pair.first; j < pair.second; j++ )
            {
                if ( whitespace.count(m_buffer[j]) == 0 )
                {
                    if ( isKey == true )
                    {
                        if ( m_buffer[j] == ':' )
                        {
                            isKey = false;
                        }
                        else
                        {
                            key += m_buffer[j];
                        }
                    }
                    else
                    {
                        value += m_buffer[j];
                    }
                }
            }

            if ( key != "" )
            {
                m_headers[key] = value;
            }
            if ( key == "Content-Length" )
            {
                m_data.resize(static_cast<size_t>(std::stoull(value)));
            }
        }
    }

    for ( size_t i = 0; i < m_data.size(); i++ )
    {
        size_t j = dataBegin + i;
        m_data[i] = consumeChar(j);
    }
}

Method Request::method() const
{
    return m_method;
}

std::string Request::url() const
{
    return m_url;
}

Version Request::version() const
{
    return m_version;
}

std::string Request::header(const std::string& key) const
{
    auto it = m_headers.find(key);
    if ( it != m_headers.end() )
    {
        return it->second;
    }
    return std::string();
}

Request::Buffer Request::data() const
{
    return m_data;
}

char Request::consumeChar(size_t & index)
{
    if (index >= m_buffer.size())
    {
       if (false == m_connection->receive(m_buffer, 4000))
        {
            throw std::exception();
        }
    }

    return m_buffer[index++];
}

} // namespace http

} // namespace rest
