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

#include "lexer.hpp"

lexer::lexer(const anonymous_int_function & get_functor,
             const anonymous_int_function & peek_functor)
    : get_functor_(get_functor)
    , peek_functor_(peek_functor)
    , last_char_(0)
{}

int lexer::get()
{
    int result = get_functor_();
    if (result == '\r') {
        if (peek_functor_() == '\n') {
            get_functor_();
        }
        result = '\n';
    }

    if ((result == '\n') && (last_char_ != '\n')) {
        const int n = peek_functor_();
        if ((n == ' ') || (n == '\t')) {
            get_functor_();
            result = ' ';
        }
    }

    last_char_ = static_cast<char>(result);
    return result;
}

int lexer::get_non_whitespace()
{
    int result = 0;
    do {
        result = get();
    } while ((result == ' ') || (result == '\t'));
    return result;
}

int lexer::get_unsigned_integer(int & character)
{
    if ((character < '0') || (character > '9')) {
        return -1;
    }

    int result = 0;
    do {
        if (character < 0) {
            return -1;
        }
        result = (result * 10) + (character - 0x30);
        character = get();
    } while ((character >= '0') && (character <= '9'));

    return result;
}

int lexer::peek()
{
    return peek_functor_();
}
