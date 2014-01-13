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

#ifndef __LIBREST_SOCKET_INPUTSOCKETSTREAM_HPP__
#define __LIBREST_SOCKET_INPUTSOCKETSTREAM_HPP__

#include <socket/socketinterface.hpp>

namespace rest
{

namespace socket
{

class InputSocketStream
{
public:

    explicit InputSocketStream(const ConnectionPtr & connection);

    char get();
    void unget();

private:
    struct Flags
    {
        bool sane : 1;
        bool eof : 1;
        bool error : 1;
    };

    ConnectionPtr m_connection;
    Buffer m_buffer;
    size_t m_index;
    Flags m_flags;
};

//! Reads one character from the stream or '\0' in case of error or eof.
InputSocketStream & operator>>(InputSocketStream & iss, char & c);

//! Reads a whole line from the stream. A line ends on '\0', '\n', '\r' character or "\r\n"
//! sequence.
InputSocketStream & operator>>(InputSocketStream & iss, std::string & s);

} // namespace socket

} // namespace rest

#endif // __LIBREST_SOCKET_INPUTSOCKETSTREAM_HPP__
