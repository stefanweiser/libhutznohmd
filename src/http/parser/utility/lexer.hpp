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

template<typename lower_case_string>
size_t compare_lower_case_string(const lower_case_string &, int32_t & character, const lexer & l)
{
    size_t result = lower_case_string::size;
    for (size_t i = 0; i < lower_case_string::size; i++) {
        character = l.get();
        if (false == compare_case_insensitive(lower_case_string::value[i], character)) {
            break;
        }
        result--;
    }
    return result;
}

//! Tries to complete a string using a reference string and a lexer. If the comparison completes
//! it returns true and the callee can assume, that the string is equal to the reference.
//! If the function returns false, the parsed string is stored in the fail safe result. The
//! function assumes, that no new character has been lexed before entering the function. The
//! reference string must not contain any upper case letters.
template<size_t size, typename continue_function>
bool compare_to_reference(int32_t & character,
                          const std::string & ref,
                          const size_t & already_parsed,
                          push_back_string<size> & fail_safe_result,
                          const continue_function & continue_condition_functor,
                          const lexer & l)
{
    for (size_t i = already_parsed; i < ref.size(); i++) {
        character = l.get();
        if ((false == continue_condition_functor(static_cast<char>(character))) ||
            (false == compare_case_insensitive(ref[i], static_cast<char>(character)))) {
            for (size_t k = 0; k < i; k++) {
                fail_safe_result.push_back(ref[k]);
            }
            return false;
        }
    }
    character = l.get();
    return true;
}

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

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_LEXER_HPP__
