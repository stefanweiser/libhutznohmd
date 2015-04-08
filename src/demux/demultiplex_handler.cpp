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

#include <demux/demultiplexer.hpp>

#include "demultiplex_handler.hpp"

namespace hutzn
{

namespace demux
{

demultiplex_handler::demultiplex_handler(demux_disconnect_interface& demuxer,
                                         const request_handler_id& id)
    : demuxer_(demuxer)
    , id_(id)
{
}

demultiplex_handler::~demultiplex_handler()
{
    demuxer_.disconnect(id_);
}

} // namespace demux

} // namespace hutzn