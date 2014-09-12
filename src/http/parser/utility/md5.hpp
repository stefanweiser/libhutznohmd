/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREST_HTTP_PARSER_UTILITY_MD5_HPP
#define LIBREST_HTTP_PARSER_UTILITY_MD5_HPP

#include <array>
#include <cstdint>
#include <vector>

namespace rest
{

namespace http
{

//! @brief Calculates the MD5 sum of a given vector of bytes.
//! @param data Vector of data to parse.
//! @return Array of 16 bytes, that contains the digest.
//! @warning This works only for little endian architectures.
std::array<uint8_t, 16> calculate_md5(const std::vector<char>& data);

} // namespace http

} // namespace rest

#endif // LIBREST_HTTP_PARSER_UTILITY_MD5_HPP
