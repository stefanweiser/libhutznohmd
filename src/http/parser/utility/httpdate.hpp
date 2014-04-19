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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_HTTPDATE_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_HTTPDATE_HPP__

#include <cstddef>
#include <ctime>

#include <http/parser/utility/lexer.hpp>

time_t parse_timestamp(int32_t & character, const lexer & l);

#endif // __LIBREST_HTTP_PARSER_UTILITY_HTTPDATE_HPP__
