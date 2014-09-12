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

#ifndef LIBREST_HTTP_PARSER_UTILITY_BASE64_HPP
#define LIBREST_HTTP_PARSER_UTILITY_BASE64_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace rest
{

namespace http
{

static inline bool is_base64(uint8_t c)
{
    return (std::isalnum(c) || (c == '+') || (c == '/'));
}

std::string encode_base64(const std::vector<uint8_t>& data);
std::vector<uint8_t> decode_base64(const std::string& encoded_string);

} // namespace http

} // namespace rest

#endif // LIBREST_HTTP_PARSER_UTILITY_BASE64_HPP
