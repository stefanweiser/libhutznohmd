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
    explicit Request(const rest::socket::ConnectionPtr& connection);
    void parse();

private:
    enum class ParserState
    {
        Method,
        URL,
        Version,
        Headers,
        Data,
        Finished
    };

    typedef std::vector<uint8_t> Buffer;

    uint8_t getChar();
    ParserState parseMethod();
    ParserState parseURL();
    ParserState parseVersion();
    ParserState parseHeaders();
    ParserState parseData();
    ParserState parseFinished();

    rest::socket::ConnectionPtr m_connection;
    Buffer m_buffer;
    size_t m_currentIndex;

    Method m_method;
    std::string m_url;
    Version m_version;
    std::vector<std::tuple<std::string, std::string>> m_headers;
    Buffer m_data;
};

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_REQUEST_HPP__
