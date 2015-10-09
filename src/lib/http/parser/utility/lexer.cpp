/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

 * The libhutznohmd project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The libhutznohmd project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the libhutznohmd project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "lexer.hpp"

namespace hutzn
{

namespace http
{

lexer::lexer(const anonymous_int_function& get_functor,
             const anonymous_int_function& peek_functor)
    : get_functor_(get_functor)
    , peek_functor_(peek_functor)
    , last_char_(0)
{
}

int32_t lexer::get() const
{
    int32_t result = get_functor_();
    if (result == '\r') {
        if (peek_functor_() == '\n') {
            get_functor_();
        }
        result = '\n';
    }

    if ((result == '\n') && (last_char_ != '\n')) {
        const int32_t n = peek_functor_();
        if ((n == ' ') || (n == '\t')) {
            get_functor_();
            result = ' ';
        }
    }

    last_char_ = static_cast<char_t>(result);
    return result;
}

int32_t lexer::get_non_whitespace() const
{
    int32_t result = 0;
    do {
        result = get();
    } while ((result == ' ') || (result == '\t'));
    return result;
}

int32_t lexer::get_unsigned_integer(int32_t& character) const
{
    if ((character < '0') || (character > '9')) {
        return -1;
    }

    int32_t result = 0;
    do {
        if (character < 0) {
            return -1;
        }

        int32_t old_result = result;
        result = (result * 10) + (character - 0x30);

        if (old_result > result) {
            // Overflow.
            return -1;
        }

        character = get();
    } while ((character >= '0') && (character <= '9'));

    return result;
}

bool parse_comment(int32_t& character, const lexer& l)
{
    // First character must be an opening parenthesis.
    if (character != '(') {
        return false;
    }

    // Go on to the first character.
    character = l.get();

    // Comments support nesting, so we have to check for that.
    size_t nesting_depth = 1;

    // Eat up the characters till a valid end is detected. This could only be
    // the end of the stream.
    while (character >= 0) {
        if ('(' == character) {
            nesting_depth++;
        } else if (')' == character) {
            nesting_depth--;
            if (0 == nesting_depth) {
                character = l.get();
                return true;
            }
        }
        character = l.get();
    }

    // Is only reached in case of the end of the stream.
    return false;
}

} // namespace http

} // namespace hutzn