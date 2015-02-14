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

#ifndef LIBHUTZNOHMD_REQEUST_PARSER_DATA_HPP
#define LIBHUTZNOHMD_REQEUST_PARSER_DATA_HPP

#include <map>

#include <libhutznohmd/demux.hpp>
#include <utility/trie.hpp>

namespace hutzn
{

namespace request
{

class parser_data
{
public:
    explicit parser_data();

    hutzn::request::mime_type register_mime_type(const std::string& type);
    hutzn::request::mime_subtype
    register_mime_subtype(const std::string& subtype);
    bool unregister_mime_type(const hutzn::request::mime_type& type);
    bool unregister_mime_subtype(const hutzn::request::mime_subtype& subtype);
    hutzn::request::mime_type parse_mime_type(const char* const string,
                                              const size_t length);
    hutzn::request::mime_subtype parse_mime_subtype(const char* const string,
                                                    const size_t length);

private:
    uint8_t next_mime_type_value_;
    uint16_t next_mime_subtype_value_;
    std::map<mime_type, std::string> registered_mime_types_;
    std::map<mime_subtype, std::string> registered_mime_subtypes_;
    trie<mime_type> mime_types_;
    trie<mime_subtype> mime_subtypes_;
};

} // namespace request

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQEUST_PARSER_DATA_HPP
