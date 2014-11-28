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

#ifndef LIBHUTZNOHMD_HTTP_PARSER_UTILITY_TIMESTAMP_HPP
#define LIBHUTZNOHMD_HTTP_PARSER_UTILITY_TIMESTAMP_HPP

#include <cstddef>
#include <ctime>

#include <http/parser/utility/lexer.hpp>

namespace rest
{

namespace http
{

time_t parse_timestamp(int32_t& character, const lexer& l);

} // namespace http

} // namespace rest

#endif // LIBHUTZNOHMD_HTTP_PARSER_UTILITY_TIMESTAMP_HPP
