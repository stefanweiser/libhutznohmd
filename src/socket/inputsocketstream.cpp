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

#include "inputsocketstream.hpp"

namespace rest
{

namespace socket
{

InputSocketStream::InputSocketStream(const ConnectionPtr & connection)
    : m_connection(connection)
    , m_buffer()
    , m_index(0)
    , m_flags({false, false, false})
{
    if (m_connection)
    {
        m_flags.sane = true;
    }
    else
    {
        m_flags.error = true;
    }
}

char InputSocketStream::get()
{
    if ((m_flags.error == true) || (m_flags.eof == true))
    {
        return '\0';
    }

    if (m_index == m_buffer.size())
    {
        const bool received = m_connection->receive(m_buffer, 4000);
        if (false == received)
        {
            m_flags.sane = false;
            m_flags.eof = true;
            m_flags.error = false;
            return '\0';
        }
    }

    return m_buffer[m_index++];
}

void InputSocketStream::unget()
{
    if ((m_index == 0) || (m_flags.error == true))
    {
        return;
    }

    m_index--;
}

InputSocketStream & operator>>(InputSocketStream & is, char & c)
{
    c = is.get();
    return is;
}

} // namespace socket

} // namespace rest
