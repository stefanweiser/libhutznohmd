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

#include "mime_handler.hpp"

#include "utility/select_char_map.hpp"

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
    // registration has to be guarded
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_types_.register_type(type);
}

mime_subtype mime_handler::register_mime_subtype(const std::string& subtype)
{
    // registration has to be guarded
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_subtypes_.register_type(subtype);
}

bool mime_handler::unregister_mime_type(const mime_type& type)
{
    // unregistration has to be guarded
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_types_.unregister_type(type);
}

bool mime_handler::unregister_mime_subtype(const mime_subtype& subtype)
{
    // unregistration has to be guarded
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return mime_subtypes_.unregister_type(subtype);
}

bool mime_handler::are_two_types_valid(const mime& type1,
                                       const mime& type2) const
{
    // check for validity has to be guarded
    std::lock_guard<std::mutex> lock(mime_type_mutex_);
    return is_mime_valid(type1) && is_mime_valid(type2);
}

mime mime_handler::parse(const char_t* const data,
                         const size_t max_length) const
{
    // even the construction of the whitespace map has to be guarded
    std::lock_guard<std::mutex> lock(mime_type_mutex_);

    // stores all characters that are considered as whitespace
    static const select_char_map whitespace_map =
        make_select_char_map(' ', '\t', '\n', '\r');

    // search for the slash which splits type and subtype
    const char_t* string = data;
    size_t remaining = max_length;
    while ((remaining > 0) && ('/' != (*string))) {
        string++;
        remaining--;
    }

    // store the type begin and its length
    const char_t* const type_begin = data;
    const size_t type_size = max_length - remaining;

    // skip the slash
    if (remaining > 0) {
        string++;
        remaining--;
    }

    // store the potential subtype begin
    const char_t* const subtype_begin = string;

    // search for a whitespace
    while ((remaining > 0) &&
           (!whitespace_map[static_cast<uint8_t>(*string)])) {
        string++;
        remaining--;
    }

    // calculate the subtype size
    const size_t subtype_size = static_cast<size_t>(string - subtype_begin);

    // check whether type and subtype are registered
    const mime_type type = mime_types_.parse_type(type_begin, type_size);
    const mime_subtype subtype =
        mime_subtypes_.parse_type(subtype_begin, subtype_size);

    // return the result
    return mime(type, subtype);
}

bool mime_handler::is_both_unset_or_set(const mime& t) const
{
    const mime_type type = t.first;
    const mime_subtype subtype = t.second;

    // either both are none or both are not none
    bool result;
    if (((type == mime_type::NONE) && (subtype == mime_subtype::NONE)) ||
        ((type != mime_type::NONE) && (subtype != mime_subtype::NONE))) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

bool mime_handler::is_mime_valid(const mime& t) const
{
    const mime_type type = t.first;
    const mime_subtype subtype = t.second;

    bool result = is_both_unset_or_set(t);

    // of course invalid values are not valid
    if ((type == mime_type::INVALID) || (subtype == mime_subtype::INVALID)) {
        result = false;
    }

    // valid values must be registered
    // unset types are unregistered but also valid
    if (((!mime_types_.is_registered(type)) ||
         (!mime_subtypes_.is_registered(subtype))) &&
        (type != mime_type::NONE) && (subtype != mime_subtype::NONE)) {
        result = false;
    }

    return result;
}

} // namespace hutzn
