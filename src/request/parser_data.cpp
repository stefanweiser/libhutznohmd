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
    : next_mime_type_value_(1)
    , next_mime_subtype_value_(1)
    , mime_types_(true)
    , mime_subtypes_(true)
{
    static_assert(sizeof(hutzn::request::mime_type) ==
                      sizeof(decltype(next_mime_type_value_)),
                  "Mime type and next value is not of the same type");
    static_assert(sizeof(hutzn::request::mime_subtype) ==
                      sizeof(decltype(next_mime_subtype_value_)),
                  "Mime subtype and next value is not of the same type");
}

hutzn::request::mime_type
parser_data::register_mime_type(const std::string& type)
{
    if (0 == next_mime_type_value_) {
        return hutzn::request::mime_type::INVALID;
    }

    hutzn::request::mime_type value =
        static_cast<hutzn::request::mime_type>(next_mime_type_value_);
    if (true == mime_types_.insert(type.c_str(), value)) {
        next_mime_type_value_++;
        registered_mime_types_[value] = type;
        return value;
    }
    return hutzn::request::mime_type::INVALID;
}

hutzn::request::mime_subtype
parser_data::register_mime_subtype(const std::string& subtype)
{
    if (0 == next_mime_subtype_value_) {
        return hutzn::request::mime_subtype::INVALID;
    }

    hutzn::request::mime_subtype value =
        static_cast<hutzn::request::mime_subtype>(next_mime_subtype_value_);
    if (true == mime_subtypes_.insert(subtype.c_str(), value)) {
        next_mime_subtype_value_++;
        registered_mime_subtypes_[value] = subtype;
        return value;
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
