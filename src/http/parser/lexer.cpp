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

class lexer;
class data;

}

}

typedef struct httpscan {
    rest::http::lexer * lexer;
    rest::http::data * data;
} httpscan_t;

namespace rest
{

namespace http
{

lexer::lexer(const std::function<int()> & get_functor, const std::function<int()> & peek_functor)
    : get_functor_(get_functor)
    , peek_functor_(peek_functor)
    , last_character_(0)
    , finished_(false)
{}

void lexer::finish()
{
    finished_ = true;
}

bool lexer::finished() const
{
    return finished_;
}

void lexer::error(const char * /*error_string*/)
{}

int lexer::get()
{
    if (true == finished_) {
        return -1;
    }

    int result = get_functor_();
    if (result == '\r') {
        if (peek_functor_() == '\n') {
            get_functor_();
        }
        result = '\n';
    }

    if ((result == '\n') && (last_character_ != '\n')) {
        if ((peek_functor_() == ' ') || (peek_functor_() == '\t')) {
            get_functor_();
            result = ' ';
        }
    }

    last_character_ = static_cast<char>(result);
    return result;
}

} // namespace http

} // namespace rest



int httplex(int * lval, httpscan_t * scanner)
{
    int result = scanner->lexer->get();
    *lval = result;
    return result;
}

void httperror(httpscan_t * scanner, const char * string)
{
    scanner->lexer->error(string);
}

void http_finish(httpscan_t * scanner)
{
    scanner->lexer->finish();
}

