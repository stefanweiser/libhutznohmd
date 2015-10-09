/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_REQUEST_BASE64_HPP
#define LIBHUTZNOHMD_REQUEST_BASE64_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace hutzn
{

//! Encodes data in base64 format based defined in
//! [RFC4648|http://tools.ietf.org/html/rfc4648] and returns the encoded
//! string. The characters 62 and 63 are '+' and '/'. The encoding doesn't
//! have lines separated and uses padding characters at the end.
std::string encode_base64(const std::vector<uint8_t>& data);

//! Decodes a base64 formatted string into binary data. It will act a bit
//! more tolerant than [RFC4648|http://tools.ietf.org/html/rfc4648] in case
//! of invalid characters, line separators, char 62 and char 63 encodings.
//! It will accept '+' and '-' as character 62 and therefore accepts also '/'
//! and '_' for character 63. It will skip any characters, that are not part
//! of the encoding.
std::vector<uint8_t> decode_base64(const std::string& encoded_string);

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_BASE64_HPP
