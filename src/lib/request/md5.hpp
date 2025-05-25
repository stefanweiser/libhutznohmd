/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_REQUEST_MD5_HPP
#define LIBHUTZNOHMD_REQUEST_MD5_HPP

#include <array>
#include <cstdint>

#include "libhutznohmd/types.hpp"

namespace hutzn
{

//! Number of bytes in a md5 sum.
static const std::size_t md5_size = 16;

//! Type that stores a md5 sum.
using md5_array = std::array<uint8_t, md5_size>;

//! @brief Calculates the MD5 sum of a given vector of bytes.
//!
//! @param[in] data Pointer to the data to calculate.
//! @param[in] size Size of the data.
//! @return         Array of 16 bytes, that contains the digest.
//! @warning        This currently works only on little endian architectures.
md5_array calculate_md5(const char_t* const data, const std::size_t size);

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_MD5_HPP
