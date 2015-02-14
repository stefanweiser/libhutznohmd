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

parser_data::parser_data()
    : mime_types_(true)
    , mime_subtypes_(true)
{
}

hutzn::request::mime_type
parser_data::register_mime_type(const std::string& type)
{
    hutzn::request::mime_type result = hutzn::request::mime_type::INVALID;
    if (true == mime_types_.insert(type.c_str(), result)) {
        registered_mime_types_[result] = type;
        return result;
    }
    return hutzn::request::mime_type::INVALID;
}

hutzn::request::mime_subtype
parser_data::register_mime_subtype(const std::string& subtype)
{
    hutzn::request::mime_subtype result = hutzn::request::mime_subtype::INVALID;
    if (true == mime_subtypes_.insert(subtype.c_str(), result)) {
        registered_mime_subtypes_[result] = subtype;
        return result;
    }
    return hutzn::request::mime_subtype::INVALID;
}

bool parser_data::unregister_mime_type(const hutzn::request::mime_type& type)
{
    auto it = registered_mime_types_.find(type);
    if (it != registered_mime_types_.end()) {
        const bool result = mime_types_.erase(it->second.c_str());

        // The registration of that type is based on the existence in the trie
        // and not in the map. Therefore ignoring this result is mandatory.
        registered_mime_types_.erase(it);
        return result;
    }
    return false;
}

bool parser_data::unregister_mime_subtype(
    const hutzn::request::mime_subtype& subtype)
{
    auto it = registered_mime_subtypes_.find(subtype);
    if (it != registered_mime_subtypes_.end()) {
        const bool result = mime_subtypes_.erase(it->second.c_str());

        // The registration of that type is based on the existence in the trie
        // and not in the map. Therefore ignoring this result is mandatory.
        registered_mime_subtypes_.erase(it);
        return result;
    }
    return false;
}

hutzn::request::mime_type parser_data::parse_mime_type(const char* const string,
                                                       const size_t length)
{
    bool success;
    hutzn::request::mime_type result;
    std::tie(success, result) = mime_types_.find(string, length);
    if (true == success) {
        return result;
    }
    return hutzn::request::mime_type::INVALID;
}

hutzn::request::mime_subtype
parser_data::parse_mime_subtype(const char* const string, const size_t length)
{
    bool success;
    hutzn::request::mime_subtype result;
    std::tie(success, result) = mime_subtypes_.find(string, length);
    if (true == success) {
        return result;
    }
    return hutzn::request::mime_subtype::INVALID;
}

} // namespace request

} // namespace hutzn
