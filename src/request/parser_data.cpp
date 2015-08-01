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

#include <cassert>

#include "parser_data.hpp"

namespace hutzn
{

parser_data::parser_data(void)
    : mime_types_()
    , mime_subtypes_()
{
    const size_t type_size = static_cast<size_t>(mime_type::COUNT) -
                             mime_data<mime_type, uint8_t>::special_value_count;
    static const char_t* const type_names[] = {"*",     "application", "audio",
                                               "image", "text",        "video"};
    static_assert((sizeof(type_names) / sizeof(*type_names)) == type_size,
                  "Sizes of mime type and the type strings does not match.");

    // The invalid value is getting ignored.
    for (uint8_t i = 0; i < type_size; i++) {
        const mime_type t = mime_types_.register_type(type_names[i]);
        assert(static_cast<size_t>(t) ==
               (i + mime_data<mime_type, uint8_t>::special_value_count));
        UNUSED(t);
    }

    const size_t subtype_size =
        static_cast<size_t>(mime_subtype::COUNT) -
        mime_data<mime_subtype, uint16_t>::special_value_count;
    static const char_t* const subtype_names[] = {"*", "plain"};
    static_assert(
        (sizeof(subtype_names) / sizeof(*subtype_names)) == subtype_size,
        "Sizes of mime subtype and the subtype strings does not match.");

    // The invalid value is getting ignored.
    for (uint8_t i = 0; i < subtype_size; i++) {
        mime_subtypes_.register_type(subtype_names[i]);
    }
}

mime_type parser_data::register_mime_type(const std::string& type)
{
    return mime_types_.register_type(type);
}

mime_subtype parser_data::register_mime_subtype(const std::string& subtype)
{
    return mime_subtypes_.register_type(subtype);
}

bool parser_data::unregister_mime_type(const mime_type& type)
{
    return mime_types_.unregister_type(type);
}

bool parser_data::unregister_mime_subtype(const mime_subtype& subtype)
{
    return mime_subtypes_.unregister_type(subtype);
}

mime_type parser_data::parse_mime_type(const char_t* const string,
                                       const size_t max_length)
{
    return mime_types_.parse_type(string, max_length);
}

mime_subtype parser_data::parse_mime_subtype(const char_t* const string,
                                             const size_t max_length)
{
    return mime_subtypes_.parse_type(string, max_length);
}

bool parser_data::is_mime_type_registered(const mime_type& type) const
{
    return mime_types_.is_registered(type);
}

bool parser_data::is_mime_subtype_registered(const mime_subtype& subtype) const
{
    return mime_subtypes_.is_registered(subtype);
}

} // namespace hutzn
