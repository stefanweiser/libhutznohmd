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

#include "request.hpp"

namespace rest
{

namespace http
{

Request::Request(const rest::socket::ConnectionPtr& connection)
    : m_connection(connection)
    , m_buffer()
    , m_currentIndex(0)
    , m_method(Method::UNKNOWN)
    , m_url()
    , m_version(Version::HTTP_UNKNOWN)
    , m_headers()
    , m_data()
{}

void Request::parse()
{
    ParserState state = ParserState::Method;
    while ( (state != ParserState::Finished) && (state != ParserState::Error) )
    {
        switch ( state )
        {
        case ParserState::Method:
            state = parseMethod();
            break;

        case ParserState::URL:
            state = parseURL();
            break;

        case ParserState::Version:
            state = parseVersion();
            break;

        case ParserState::Headers:
            state = parseHeaders();
            break;

        case ParserState::Data:
            state = parseData();
            break;

        case ParserState::Finished:
        case ParserState::Error:
        default:
            break;
        }
    }
}

char Request::getChar()
{
    if ( m_currentIndex < m_buffer.size() )
    {
        return m_buffer[m_currentIndex++];
    }

    if ( false == m_connection->receive(m_buffer, 4000) )
    {
        throw std::exception();
    }

    m_currentIndex = 1;
    return m_buffer[0];
}

Request::ParserState Request::parseMethod()
{
    char c = getChar();
    if ( c == 'H' )
    {
        if ( getChar() != 'E' ) { return ParserState::Error; }
        if ( getChar() != 'A' ) { return ParserState::Error; }
        if ( getChar() != 'D' ) { return ParserState::Error; }
        m_method = Method::HEAD;
    }
    else if ( c == 'G' )
    {
        if ( getChar() != 'E' ) { return ParserState::Error; }
        if ( getChar() != 'T' ) { return ParserState::Error; }
        m_method = Method::GET;
    }
    else if ( c == 'D' )
    {
        if ( getChar() != 'E' ) { return ParserState::Error; }
        if ( getChar() != 'L' ) { return ParserState::Error; }
        if ( getChar() != 'E' ) { return ParserState::Error; }
        if ( getChar() != 'T' ) { return ParserState::Error; }
        if ( getChar() != 'E' ) { return ParserState::Error; }
        m_method = Method::DELETE;
    }
    else if ( c == 'C' )
    {
        if ( getChar() != 'O' ) { return ParserState::Error; }
        if ( getChar() != 'N' ) { return ParserState::Error; }
        if ( getChar() != 'N' ) { return ParserState::Error; }
        if ( getChar() != 'E' ) { return ParserState::Error; }
        if ( getChar() != 'C' ) { return ParserState::Error; }
        if ( getChar() != 'T' ) { return ParserState::Error; }
        m_method = Method::CONNECT;
    }
    else if ( c == 'O' )
    {
        if ( getChar() != 'P' ) { return ParserState::Error; }
        if ( getChar() != 'T' ) { return ParserState::Error; }
        if ( getChar() != 'I' ) { return ParserState::Error; }
        if ( getChar() != 'O' ) { return ParserState::Error; }
        if ( getChar() != 'N' ) { return ParserState::Error; }
        if ( getChar() != 'S' ) { return ParserState::Error; }
        m_method = Method::OPTIONS;
    }
    else if ( c == 'T' )
    {
        if ( getChar() != 'R' ) { return ParserState::Error; }
        if ( getChar() != 'A' ) { return ParserState::Error; }
        if ( getChar() != 'C' ) { return ParserState::Error; }
        if ( getChar() != 'E' ) { return ParserState::Error; }
        m_method = Method::TRACE;
    }
    else if ( c == 'P' )
    {
        c = getChar();
        if ( c == 'O' )
        {
            if ( getChar() != 'S' ) { return ParserState::Error; }
            if ( getChar() != 'T' ) { return ParserState::Error; }
            m_method = Method::POST;
        }
        else if ( c == 'U' )
        {
            if ( getChar() != 'T' ) { return ParserState::Error; }
            m_method = Method::PUT;
        }
        else
        {
            return ParserState::Error;
        }
    }
    else
    {
        return ParserState::Error;
    }

    return ParserState::URL;
}

Request::ParserState Request::parseURL()
{
    char c = getChar();
    while ( (c == ' ') && (c == '\t') ) { c = getChar(); }
    while ( (c != ' ') || (c != '\t') )
    {
        if ( c == '%' )
        {
            char d = getChar();
            char e = getChar();
            if ( !std::isxdigit(d) || !std::isxdigit(e) )
            {
                return ParserState::Error;
            }
            int f = (d > '9') ? ((d &~ 0x20) - 'A' + 10) : (d - '0');
            int g = (e > '9') ? ((e &~ 0x20) - 'A' + 10) : (e - '0');
            c = static_cast<char>((f << 4) | g);
        }
        else if ( (c == '\n') || (c == '\r') )
        {
            return ParserState::Error;
        }

        m_url += c;
        c = getChar();
    }

    return ParserState::Finished;
}

Request::ParserState Request::parseVersion()
{
    char c = getChar();
    while ( (c == ' ') && (c == '\t') ) { c = getChar(); }
    if ( c != 'H' ) { return ParserState::Error; }
    if ( getChar() != 'T' ) { return ParserState::Error; }
    if ( getChar() != 'T' ) { return ParserState::Error; }
    if ( getChar() != 'P' ) { return ParserState::Error; }
    if ( getChar() != '/' ) { return ParserState::Error; }
    if ( getChar() != '1' ) { return ParserState::Error; }
    if ( getChar() != '.' ) { return ParserState::Error; }

    c = getChar();
    if ( c == '0' )
    {
        m_version = Version::HTTP_1_0;
    }
    else if ( c == '1' )
    {
        m_version = Version::HTTP_1_1;
    }
    else
    {
        return ParserState::Error;
    }

    return ParserState::Headers;
}

Request::ParserState Request::parseHeaders()
{
    return ParserState::Finished;
}

Request::ParserState Request::parseData()
{
    return ParserState::Finished;
}

} // namespace http

} // namespace rest
