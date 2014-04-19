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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_ANONYMOUSINTFUNCTION_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_ANONYMOUSINTFUNCTION_HPP__

class anonymous_int_function
{
public:
    explicit anonymous_int_function(int (*functor)(void *), void * handle);
    int operator()() const;
    anonymous_int_function(const anonymous_int_function & rhs);
    anonymous_int_function & operator=(const anonymous_int_function & rhs);

private:
    int (*functor_)(void *);
    void * handle_;
};

inline anonymous_int_function::anonymous_int_function(int (*functor)(void *), void * handle)
    : functor_(functor)
    , handle_(handle)
{}

inline int anonymous_int_function::operator()() const
{
    return functor_(handle_);
}

inline anonymous_int_function::anonymous_int_function(const anonymous_int_function & rhs)
    : functor_(rhs.functor_)
    , handle_(rhs.handle_)
{}

inline anonymous_int_function & anonymous_int_function::operator=(const anonymous_int_function &
        rhs)
{
    functor_ = rhs.functor_;
    handle_ = rhs.handle_;
    return *this;
}

#endif // __LIBREST_HTTP_PARSER_UTILITY_ANONYMOUSINTFUNCTION_HPP__
