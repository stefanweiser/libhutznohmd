/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBREST_HTTP_PARSER_UTILITY_LEXER_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_LEXER_HPP__

#include <cstddef>
#include <string>

#include <http/parser/utility/anonymousintfunction.hpp>
#include <http/parser/utility/charactercompare.hpp>
#include <http/parser/utility/pushbackstring.hpp>

namespace rest
{

namespace http
{

class lexer
{
public:
    explicit lexer(const anonymous_int_function & get_functor,
                   const anonymous_int_function & peek_functor);

    int32_t get() const;
    int32_t get_non_whitespace() const;
    int32_t get_unsigned_integer(int32_t & character) const;

private:
    const anonymous_int_function get_functor_;
    const anonymous_int_function peek_functor_;
    mutable char last_char_;
};

//! Parses a word utilizing a transformations functor and stopping if the continue condition gets
//! wrong.
template<size_t size, typename transformation_function, typename continue_function>
void parse_word(int32_t & character,
                push_back_string<size> & result,
                const transformation_function & transformation_functor,
                const continue_function & continue_condition_functor,
                const lexer & l)
{
    while ((character >= 0) &&
           (true == continue_condition_functor(static_cast<char>(character)))) {
        result.push_back(transformation_functor(static_cast<char>(character)));
        character = l.get();
    }
}

//! Parses a quoted string stopping if an invalid character is found or the end is found.
//! @return True if the string was successfully parsed and false if an error occurred.
template<size_t size>
bool parse_quoted_string(int32_t & character,
                         push_back_string<size> & result,
                         const lexer & l)
{
    // First character must be a quote character.
    if (character != '"') {
        return false;
    }

    // Go on to the first character.
    character = l.get();

    // Fill the result string till a valid end is detected. This could be
    // 1. the end of the stream or
    // 2. an invalid character.
    while ((character >= 0) &&
           (true == is_valid_quoted_string_character(static_cast<uint8_t>(character)))) {
        result.push_back(static_cast<char>(character));
        character = l.get();
    }

    // Last character must be a quote character.
    if (character != '"') {
        return false;
    }

    // Go on one character.
    character = l.get();
    return true;
}

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_LEXER_HPP__
