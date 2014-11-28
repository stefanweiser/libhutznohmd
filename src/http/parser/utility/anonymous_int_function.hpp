/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_HTTP_PARSER_UTILITY_ANONYMOUS_INT_FUNCTION_HPP
#define LIBHUTZNOHMD_HTTP_PARSER_UTILITY_ANONYMOUS_INT_FUNCTION_HPP

#include <cstdint>

namespace rest
{

namespace http
{

//! Needed by the lexer for requesting a symbol. Hides a handle that is used to
// call back into an
//! object.
class anonymous_int_function
{
public:
    //! First it needs a function pointer that takes a handle and returns a
    // symbol. Second it
    //! needs the handle that is transmitted with any call.
    explicit anonymous_int_function(int32_t (*functor)(void*), void* handle);

    //! Calls the function pointer and returns a symbol.
    int32_t operator()() const;

private:
    int32_t (*functor_)(void*);
    void* handle_;
};

inline anonymous_int_function::anonymous_int_function(int32_t (*functor)(void*),
                                                      void* handle)
    : functor_(functor)
    , handle_(handle)
{
}

inline int32_t anonymous_int_function::operator()() const
{
    return functor_(handle_);
}

} // namespace http

} // namespace rest

#endif // LIBHUTZNOHMD_HTTP_PARSER_UTILITY_ANONYMOUS_INT_FUNCTION_HPP
