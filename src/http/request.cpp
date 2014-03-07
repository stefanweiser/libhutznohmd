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
    , m_method(Method::UNKNOWN)
    , m_url()
    , m_version(Version::HTTP_UNKNOWN)
    , m_headers()
    , m_data()
    , m_emptyString()
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
                if (c == '\0')
                {
                    return;
                }
                if (c != '\r')
                {
                    emptyLine = false;
                }
                c = consumeChar(i);
            }
            lineIndices.push_back(std::make_pair(begin, i));
        }
        while (false == emptyLine);
        dataBegin = i;
    }

    std::string lastKey;
    for (size_t i = 0; i < lineIndices.size(); i++)
    {
        const auto & pair = lineIndices[i];
        if (i == 0)
        {
            // Request-Head
            std::vector<std::string> words;
            std::string head;
            for (size_t j = pair.first; j < pair.second; j++)
            {
                head += m_buffer[j];
            }
            std::istringstream iss(head);
            std::copy(std::istream_iterator<std::string>(iss),
                      std::istream_iterator<std::string>(),
                      std::back_inserter<std::vector<std::string>>(words));
            if (words.size() != 3)
            {
                return;
            }

            m_method = parseMethod(words[0]);
            m_url = words[1];
            m_version = parseVersion(words[2]);
        }
        else
        {
            // Reqeust body
            std::string key;
            std::string value;
            std::set<char> whitespace = { '\t', ' ', '\n', '\r' };
            bool isKey = true;
            if ((m_buffer[pair.first] == ' ') || (m_buffer[pair.first] == '\t'))
            {
                if (lastKey == "")
                {
                    return;
                }

                for (size_t j = pair.first; j < pair.second; j++)
                {
                    if (whitespace.count(m_buffer[j]) == 0)
                    {
                        value += m_buffer[j];
                    }
                }

                m_headers[lastKey] += value;
            }
            else
            {
                for (size_t j = pair.first; j < pair.second; j++)
                {
                    if (whitespace.count(m_buffer[j]) == 0)
                    {
                        if (isKey == true)
                        {
                            if (m_buffer[j] == ':')
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

                if (key != "")
                {
                    std::transform(key.begin(), key.end(), key.begin(), tolower);
                    m_headers[key] = value;
                    lastKey = key;
                }
                if (key == "content-length")
                {
                    m_data.resize(static_cast<size_t>(std::stoull(value)));
                }
            }
        }
    }

    for (size_t i = 0; i < m_data.size(); i++)
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

const std::string & Request::header(const std::string & key) const
{
    std::string _key = key;
    std::transform(_key.begin(), _key.end(), _key.begin(), tolower);
    auto it = m_headers.find(_key);
    if (it == m_headers.end())
    {
        return m_emptyString;
    }
    return it->second;
}

rest::Buffer Request::data() const
{
    return m_data;
}

char Request::consumeChar(size_t & index)
{
    if (index >= m_buffer.size())
    {
        if (false == m_connection->receive(m_buffer, 4000))
        {
            return '\0';
        }
    }

    return m_buffer[index++];
}

Method Request::parseMethod(const std::string & word)
{
    if (word == "OPTIONS")
    {
        return Method::OPTIONS;
    }
    else if (word == "GET")
    {
        return Method::GET;
    }
    else if (word == "HEAD")
    {
        return Method::HEAD;
    }
    else if (word == "POST")
    {
        return Method::POST;
    }
    else if (word == "PUT")
    {
        return Method::PUT;
    }
    else if (word == "DELETE")
    {
        return Method::DELETE;
    }
    else if (word == "TRACE")
    {
        return Method::TRACE;
    }
    else if (word == "CONNECT")
    {
        return Method::CONNECT;
    }
    else
    {
        return Method::UNKNOWN;
    }
}

Version Request::parseVersion(const std::string & word)
{
    if (word == "HTTP/1.0")
    {
        return Version::HTTP_1_0;
    }
    else if (word == "HTTP/1.1")
    {
        return Version::HTTP_1_1;
    }
    else
    {
        return Version::HTTP_UNKNOWN;
    }
}

} // namespace http

} // namespace rest
