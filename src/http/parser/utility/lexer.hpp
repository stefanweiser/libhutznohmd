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

#include <http/parser/utility/anonymousintfunction.hpp>

class lexer
{
public:
    explicit lexer(const anonymous_int_function & get_functor,
                   const anonymous_int_function & peek_functor);

    int get() const;
    int get_non_whitespace() const;
    int get_unsigned_integer(int & character) const;

private:
    const anonymous_int_function get_functor_;
    const anonymous_int_function peek_functor_;
    mutable char last_char_;
};

template<typename lower_case_string>
bool verify_forced_characters(const lower_case_string &, const lexer & l)
{
    for (size_t i = 0; i < lower_case_string::size; i++) {
        const int character = l.get();
        if (false == compare_case_insensitive(lower_case_string::value[i], character)) {
            return false;
        }
    }
    return true;
}

#endif // __LIBREST_HTTP_PARSER_UTILITY_LEXER_HPP__
