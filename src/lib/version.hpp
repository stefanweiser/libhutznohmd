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

#ifndef LIBHUTZNOHMD_VERSION_HPP
#define LIBHUTZNOHMD_VERSION_HPP

#include <string>

namespace hutzn
{

//! @brief Returns the version number as a string.
//!
//! The library version is configured by cmake.
//! @return Returns a string of the current version.
std::string get_version(void);

} // namespace hutzn

#endif // LIBHUTZNOHMD_VERSION_HPP
