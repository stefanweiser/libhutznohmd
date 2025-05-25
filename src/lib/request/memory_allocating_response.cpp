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

#include "memory_allocating_response.hpp"

namespace hutzn
{

memory_allocating_response::memory_allocating_response(
    const connection_ptr& connection)
    : connection_(connection)
{
}

bool memory_allocating_response::set_header(const char_t* const /*name*/,
                                            const char_t* const /*value*/)
{
    return false;
}

void memory_allocating_response::set_content(const buffer& /*content*/,
                                             const bool /*set_md5*/)
{
}

void memory_allocating_response::set_content_location(
    const char_t* const /*content_location*/)
{
}

void memory_allocating_response::set_location(const char_t* const /*location*/)
{
}

bool memory_allocating_response::set_retry_after(const time_t /*retry_time*/)
{
    return false;
}

void memory_allocating_response::set_server(const char_t* const /*fingerprint*/)
{
}

} // namespace hutzn
