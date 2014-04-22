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

template<size_t size>
bool parse_string_against_reference(int32_t & character,
                                    const std::string & already_parsed_string,
                                    const std::string & ref,
                                    push_back_string<size> & result,
                                    const lexer & lexer)
{
    size_t i = 0;
    size_t j = 0;
    character = lexer.get();
    while ((character >= 0) &&
           (true == is_valid_token_character(static_cast<char>(character)))) {
        char c = static_cast<char>(character);
        if ((i == j) && ((i >= ref.size()) || (true == compare_case_insensitive(ref[i], c)))) {
            i++;
        } else {
            if (true == result.empty()) {
                for (size_t k = 0; k < already_parsed_string.size(); k++) {
                    result.push_back(already_parsed_string[k]);
                }
                for (size_t k = 0; k < i; k++) {
                    result.push_back(ref[k]);
                }
            }
            result.push_back(c);
        }
        j++;
        character = lexer.get();
    }

    if (i == j) {
        if (i < ref.size()) {
            // There is missing something.
            for (size_t k = 0; k < already_parsed_string.size(); k++) {
                result.push_back(already_parsed_string[k]);
            }
            for (size_t k = 0; k < i; k++) {
                result.push_back(ref[k]);
            }
            return false;
        } else {
            return (i == ref.size());
        }
    }
    return false;
}

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_LEXER_HPP__
