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

#include "libhutznohmd/demux.hpp"

namespace hutzn
{

demux_query::~demux_query(void) noexcept(true)
{
}

demux::~demux(void) noexcept(true)
{
}

handler::~handler(void) noexcept(true)
{
}

callback_holder::~callback_holder(void) noexcept(true)
{
}

request_processor::~request_processor(void) noexcept(true)
{
}

} // namespace hutzn
