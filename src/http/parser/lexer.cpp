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

#include <cmath>
#include <cstdint>
#include <map>

#include "lexer.h"

namespace rest
{

namespace http
{

class Lexer;
class Data;

}

}

typedef struct httpscan {
    rest::http::Lexer * m_lexer;
    rest::http::Data * m_data;
} httpscan_t;

namespace rest
{

namespace http
{

Lexer::Lexer(const std::function<int()> & getFn, const std::function<int()> & peekFn)
    : m_getFn(getFn)
    , m_peekFn(peekFn)
    , m_lastChar(0)
    , m_finished(false)
{}

void Lexer::finish()
{
    m_finished = true;
}

bool Lexer::finished() const
{
    return m_finished;
}

void Lexer::error(const char * /*s*/)
{
}

int Lexer::get()
{
    if (true == m_finished) {
        return -1;
    }

    int c = m_getFn();
    if (c == '\r') {
        if (m_peekFn() == '\n') {
            m_getFn();
        }
        c = '\n';
    }

    if ((c == '\n') && (m_lastChar != '\n')) {
        if ((m_peekFn() == ' ') || (m_peekFn() == '\t')) {
            m_getFn();
            c = ' ';
        }
    }

    m_lastChar = static_cast<char>(c);
    return c;
}

} // namespace http

} // namespace rest



int httplex(int * lval, httpscan_t * scanner)
{
    int result = scanner->m_lexer->get();
    *lval = result;
    return result;
}

void httperror(httpscan_t * scanner, const char * s)
{
    scanner->m_lexer->error(s);
}

void httpFinish(httpscan_t * scanner)
{
    scanner->m_lexer->finish();
}

