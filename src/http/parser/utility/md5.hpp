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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_MD5_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_MD5_HPP__

#include <array>
#include <cstdint>
#include <vector>

namespace rest
{

namespace http
{

std::array<uint8_t, 16> calculate_md5(const std::vector<uint8_t> & data);

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_MD5_HPP__