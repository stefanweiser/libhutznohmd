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

#ifndef LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ACCEPT_MAP_HPP
#define LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ACCEPT_MAP_HPP

#include <map>

#include <libhutznohmd/demux.hpp>

namespace hutzn
{

class demultiplexer_accept_map
{
public:
    explicit demultiplexer_accept_map();

    size_t size() const;
    bool insert(const mime& type, const request_handler_callback& fn);
    bool erase(const mime& type);
    request_handler_callback find(const request_interface& request) const;

private:
    std::map<mime, request_handler_callback> map_;
    std::vector<mime> vector_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ACCEPT_MAP_HPP
