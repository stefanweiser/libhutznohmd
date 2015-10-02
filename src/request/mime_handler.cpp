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

#include "mime_handler.hpp"

namespace hutzn
{

mime_handler::mime_handler(void)
    : mime_type_mutex_()
    , mime_types_()
    , mime_subtypes_()
{
}

mime_type mime_handler::register_mime_type(const std::string& type)
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_types_.register_type(type);
}

mime_subtype mime_handler::register_mime_subtype(const std::string& subtype)
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_subtypes_.register_type(subtype);
}

bool mime_handler::unregister_mime_type(const mime_type& type)
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_types_.unregister_type(type);
}

bool mime_handler::unregister_mime_subtype(const mime_subtype& subtype)
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_subtypes_.unregister_type(subtype);
}

bool mime_handler::are_two_types_valid(const mime& type1,
                                       const mime& type2) const
{
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return (true == is_mime_valid(type1)) && (true == is_mime_valid(type2));
}

bool mime_handler::is_mime_valid(const mime& t) const
{
    const mime_type type = t.first;
    const mime_subtype subtype = t.second;

    bool result = true;

    // Either type and subtype is not unset or both are unset.
    if (((type == mime_type::NONE) && (subtype != mime_subtype::NONE)) ||
        ((type != mime_type::NONE) && (subtype == mime_subtype::NONE))) {
        result = false;
    }

    // Of course invalid values are not valid.
    if ((type == mime_type::INVALID) || (subtype == mime_subtype::INVALID)) {
        result = false;
    }

    // Valid values must be registered. Unset types are unregistered but also
    // valid.
    if ((false == mime_types_.is_registered(type)) ||
        (false == mime_subtypes_.is_registered(subtype))) {
        if ((type != mime_type::NONE) && (subtype != mime_subtype::NONE)) {
            result = false;
        }
    }

    return result;
}

} // namespace hutzn
