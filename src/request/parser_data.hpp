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

#ifndef LIBHUTZNOHMD_REQEUST_PARSER_DATA_HPP
#define LIBHUTZNOHMD_REQEUST_PARSER_DATA_HPP

#include <map>

#include <libhutznohmd/demux.hpp>
#include <request/mime_data.hpp>
#include <utility/trie.hpp>

namespace hutzn
{

class parser_data
{
public:
    explicit parser_data();
    mime_type register_mime_type(const std::string& type);
    mime_subtype register_mime_subtype(const std::string& subtype);
    bool unregister_mime_type(const mime_type& type);
    bool unregister_mime_subtype(const mime_subtype& subtype);
    mime_type parse_mime_type(const char* const string,
                              const size_t max_length);
    mime_subtype parse_mime_subtype(const char* const string,
                                    const size_t max_length);
    bool is_mime_type_registered(const mime_type& type) const;
    bool is_mime_subtype_registered(const mime_subtype& subtype) const;

private:
    mime_data<mime_type, uint8_t> mime_types_;
    mime_data<mime_subtype, uint16_t> mime_subtypes_;
};

using parser_data_pointer = std::shared_ptr<parser_data>;

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQEUST_PARSER_DATA_HPP
