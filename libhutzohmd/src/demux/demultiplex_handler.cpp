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

#include "demultiplex_handler.hpp"

namespace hutzn
{

demultiplex_handler::demultiplex_handler(handler_manager& demuxer,
                                         const request_handler_id& id)
    : demuxer_(demuxer)
    , id_(id)
{
}

demultiplex_handler::~demultiplex_handler(void) noexcept(true)
{
    demuxer_.disconnect(id_);
}

void demultiplex_handler::disable(void)
{
    demuxer_.disable(id_);
}

void demultiplex_handler::enable(void)
{
    demuxer_.enable(id_);
}

bool demultiplex_handler::is_enabled(void) const
{
    return demuxer_.is_enabled(id_);
}

} // namespace hutzn
