/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#include "parser_data.hpp"

namespace hutzn
{

namespace request
{

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

mime_type parser_data::parse_mime_type(const std::string& string)
{
    return mime_types_.parse_type(string);
}

mime_subtype parser_data::parse_mime_subtype(const std::string& string)
{
    return mime_subtypes_.parse_type(string);
}

} // namespace request

} // namespace hutzn
