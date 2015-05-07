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

#include "demultiplexer_ordered_mime_map.hpp"

namespace hutzn
{

demultiplexer_ordered_mime_map::demultiplexer_ordered_mime_map()
    : map_()
    , vector_()
{
}

size_t demultiplexer_ordered_mime_map::size() const
{
    return map_.size();
}

bool demultiplexer_ordered_mime_map::insert(const mime& type,
                                            const request_handler_callback& fn)
{
    if ((type.first == mime_type::WILDCARD) ||
        (type.second == mime_subtype::WILDCARD)) {
        return false;
    }

    auto it = map_.find(type);
    if (it == map_.end()) {
        map_[type] = fn;
        vector_.push_back(type);
        return true;
    }
    return false;
}

bool demultiplexer_ordered_mime_map::erase(const mime& type)
{
    auto it = map_.find(type);
    if (it != map_.end()) {
        std::remove(vector_.begin(), vector_.end(), type);
        return (map_.erase(type) > 0);
    }
    return false;
}

request_handler_callback demultiplexer_ordered_mime_map::find(
    const mime& type) const
{
    const bool has_any_wildcard = (type.first == mime_type::WILDCARD) ||
                                  (type.second == mime_subtype::WILDCARD);
    if (true == has_any_wildcard) {
        if (request_handler_callback result = find_ordered(type)) {
            return result;
        }
    } else {
        const auto accept_it = map_.find(type);
        if (accept_it != map_.end()) {
            return accept_it->second;
        }
    }
    return request_handler_callback();
}

request_handler_callback demultiplexer_ordered_mime_map::find_ordered(
    const mime& type) const
{
    for (const mime& value : vector_) {
        const bool type_equal_or_wildcard =
            (type.first == value.first) || (type.first == mime_type::WILDCARD);
        const bool subtype_equal_or_wildcard =
            (type.second == value.second) ||
            (type.second == mime_subtype::WILDCARD);

        if ((true == type_equal_or_wildcard) &&
            (true == subtype_equal_or_wildcard)) {
            auto it = map_.find(value);
            return it->second;
        }
    }
    return request_handler_callback();
}

} // namespace hutzn
