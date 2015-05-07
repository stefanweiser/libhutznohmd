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

#include <algorithm>

#include "demultiplexer_accept_map.hpp"

namespace hutzn
{

demultiplexer_accept_map::demultiplexer_accept_map()
    : map_()
    , vector_()
{
}

size_t demultiplexer_accept_map::size() const
{
    return map_.size();
}

bool demultiplexer_accept_map::insert(const mime& type,
                                      const request_handler_callback& fn)
{
    auto it = map_.find(type);
    if (it == map_.end()) {
        map_[type] = fn;
        vector_.push_back(type);
        return true;
    }
    return false;
}

bool demultiplexer_accept_map::erase(const mime& type)
{
    auto it = map_.find(type);
    if (it != map_.end()) {
        std::remove(vector_.begin(), vector_.end(), type);
        return (map_.erase(type) > 0);
    }
    return false;
}

request_handler_callback demultiplexer_accept_map::find(
    const request_interface& request) const
{
    void* handle = nullptr;
    mime accept_type;
    while (true == request.accept(handle, accept_type)) {
        const auto accept_it = map_.find(accept_type);
        if (accept_it != map_.end()) {
            return accept_it->second;
        }
    }
    return request_handler_callback();
}

} // namespace hutzn
