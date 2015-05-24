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

demultiplexer_ordered_mime_map::demultiplexer_ordered_mime_map(void)
    : map_()
    , vector_()
{
}

size_t demultiplexer_ordered_mime_map::size(void) const
{
    return map_.size();
}

bool demultiplexer_ordered_mime_map::insert(const mime& type,
                                            const request_handler_callback& fn)
{
    bool result = false;
    if ((type.first != mime_type::WILDCARD) &&
        (type.second != mime_subtype::WILDCARD)) {

        auto it = map_.find(type);
        if (it == map_.end()) {
            map_[type] = fn;
            vector_.push_back(type);
            result = true;
        }
    }
    return result;
}

bool demultiplexer_ordered_mime_map::erase(const mime& type)
{
    auto it = map_.find(type);
    bool result;
    if (it != map_.end()) {
        std::remove(vector_.begin(), vector_.end(), type);
        result = (map_.erase(type) > 0);
    } else {
        result = false;
    }
    return result;
}

request_handler_callback demultiplexer_ordered_mime_map::find(
    const mime& type) const
{
    request_handler_callback result;
    const bool has_any_wildcard = (type.first == mime_type::WILDCARD) ||
                                  (type.second == mime_subtype::WILDCARD);
    if (true == has_any_wildcard) {
        if (request_handler_callback find_result = find_ordered(type)) {
            result = find_result;
        }
    } else {
        const auto accept_it = map_.find(type);
        if (accept_it != map_.end()) {
            result = accept_it->second;
        }
    }
    return result;
}

request_handler_callback demultiplexer_ordered_mime_map::find_ordered(
    const mime& type) const
{
    request_handler_callback result;
    for (const mime& value : vector_) {
        const bool type_equal_or_wildcard =
            (type.first == value.first) || (type.first == mime_type::WILDCARD);
        const bool subtype_equal_or_wildcard =
            (type.second == value.second) ||
            (type.second == mime_subtype::WILDCARD);

        if ((true == type_equal_or_wildcard) &&
            (true == subtype_equal_or_wildcard)) {

            auto it = map_.find(value);
            result = it->second;
            break;
        }
    }
    return result;
}

} // namespace hutzn
