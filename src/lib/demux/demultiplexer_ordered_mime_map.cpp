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

#include "demultiplexer_ordered_mime_map.hpp"

#include <algorithm>
#include <cassert>

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

        auto insertion_result = map_.insert(
            std::make_pair(type, request_handler_infos{fn, 0, true}));
        if (insertion_result.second) {
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
    if ((it != map_.end()) && (0 == it->second.usage_counter)) {
        auto it2 = vector_.begin();
        while (it2 != vector_.end()) {
            if (type == (*it2)) {
                vector_.erase(it2);
            } else {
                ++it2;
            }
        }
        map_.erase(it);
        result = true;
    } else {
        result = false;
    }
    return result;
}

request_handler_callback demultiplexer_ordered_mime_map::find(mime& type) const
{
    request_handler_callback result;
    const bool has_any_wildcard = (type.first == mime_type::WILDCARD) ||
                                  (type.second == mime_subtype::WILDCARD);
    if (has_any_wildcard) {
        if (request_handler_callback find_result = find_ordered(type)) {
            result = find_result;
        }
    } else {
        const auto accept_it = map_.find(type);
        if ((accept_it != map_.end()) && (accept_it->second.is_available)) {
            result = accept_it->second.handler;
            // The type contains already the mime type of the result.
        }
    }
    return result;
}

void demultiplexer_ordered_mime_map::increase_usage_counter(const mime& type)
{
    auto it = map_.find(type);
    assert(it != map_.end());
    it->second.usage_counter++;
}

void demultiplexer_ordered_mime_map::decrease_usage_counter(const mime& type)
{
    auto it = map_.find(type);
    assert(it != map_.end());
    it->second.usage_counter--;
}

bool demultiplexer_ordered_mime_map::is_used(const mime& type) const
{
    auto it = map_.find(type);
    assert(it != map_.end());
    return 0 != it->second.usage_counter;
}

void demultiplexer_ordered_mime_map::set_availability(const mime& type,
                                                      const bool available)
{
    auto it = map_.find(type);
    assert(it != map_.end());
    it->second.is_available = available;
}

bool demultiplexer_ordered_mime_map::is_available(const mime& type) const
{
    auto it = map_.find(type);
    assert(it != map_.end());
    return it->second.is_available;
}

request_handler_callback demultiplexer_ordered_mime_map::find_ordered(
    mime& type) const
{
    request_handler_callback result;
    for (const mime& value : vector_) {
        const bool type_equal_or_wildcard =
            (type.first == value.first) || (type.first == mime_type::WILDCARD);
        const bool subtype_equal_or_wildcard =
            (type.second == value.second) ||
            (type.second == mime_subtype::WILDCARD);

        if (type_equal_or_wildcard && subtype_equal_or_wildcard) {

            auto it = map_.find(value);
            assert(it != map_.end());
            if (it->second.is_available) {
                result = it->second.handler;
                type = it->first;
                break;
            }
        }
    }
    return result;
}

} // namespace hutzn
