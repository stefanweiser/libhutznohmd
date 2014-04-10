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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_LOWER_CASE_STRING_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_LOWER_CASE_STRING_HPP__

#include <type_traits>

namespace detail
{

template<char c>
struct is_lower_char {
    using value = std::integral_constant < bool, (c >= 'a') && (c <= 'z') >;
};

template<typename s, size_t n>
struct is_lower_string {
    using value = std::integral_constant < bool, is_lower_string < s, n - 1 >::value::value &&
                  std::integral_constant < bool, (s::value[n - 1] >= 'a') && (s::value[n - 1] <= 'z') >::value >;
};

template <typename s>
struct is_lower_string<s, 0> {
    using value = std::true_type;
};

} // namespace detail

#define LOWER_CASE_STRING(S) \
    struct lower_case_string_##S \
    { \
        static constexpr const char * const value = #S; \
        static constexpr const size_t size = sizeof(#S) - 1; \
        static_assert(detail::is_lower_string<lower_case_string_##S, size>::value::value == true, \
                      "String is not lower case."); \
    }

#endif // __LIBREST_HTTP_PARSER_UTILITY_LOWER_CASE_STRING_HPP__
