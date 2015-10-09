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

#include <cmath>
#include <cstdint>
#include <limits>
#include <map>

#include <request/base64.hpp>
#include <http/parser/utility/timestamp.hpp>
#include <http/parser/utility/trie.hpp>

#include "base_parser.hpp"

namespace hutzn
{

namespace http
{

base_parser::base_parser(const anonymous_int_function& get_functor,
                         const anonymous_int_function& peek_functor)
    : lexer_(get_functor, peek_functor)
    , state_(parser_state::UNFINISHED)
    , header_key_()
    , header_value_()
    , version_(http::version::HTTP_UNKNOWN)
    , headers_()
    , content_length_(0)
    , content_type_()
    , date_(time(NULL))
    , connection_(connection_type::KEEP_ALIVE)
    , md5_()
    , has_md5_(false)
{
}

bool base_parser::parse_connection(int32_t& character)
{
    using value_info = trie<connection_type>::value_info;
    static const std::vector<value_info> types = {
        {value_info{"close", connection_type::CLOSE},
         value_info{"keep-alive", connection_type::KEEP_ALIVE},
         value_info{"persist", connection_type::KEEP_ALIVE}}};

    static const trie<connection_type> t(types, connection_type::ERROR);
    push_back_string<32> tmp;
    connection_type connection = t.parse(character, tmp, lexer_);
    if (connection != connection_type::ERROR) {
        connection_ = connection;
        return true;
    }
    return false;
}

bool base_parser::parse_content_length(int32_t& character)
{
    int32_t length = lexer_.get_unsigned_integer(character);
    if (length < 0) {
        return false;
    }

    content_length_ = static_cast<size_t>(length);
    return true;
}

namespace
{

bool is_base64(uint8_t c)
{
    return std::isalnum(c) || (c == '+') || (c == '/');
}

} // namespace

bool base_parser::parse_content_md5(int32_t& character)
{
    std::string s;
    while ((character >= 0) && ((is_base64(static_cast<uint8_t>(character))) ||
                                ('=' == character))) {
        s += static_cast<char_t>(character);
        character = lexer_.get();
    }

    std::vector<uint8_t> temp = decode_base64(s);
    if (temp.size() == 16) {
        for (size_t i = 0; i < 16; i++) {
            md5_[i] = temp[i];
        }
        has_md5_ = true;
        return true;
    }
    return false;
}

bool base_parser::parse_content_type(int32_t& character)
{
    return content_type_.parse(lexer_, character);
}

bool base_parser::parse_date(int32_t& character)
{
    time_t d = parse_timestamp(character, lexer_);
    if (d < 0) {
        return false;
    }

    date_ = d;
    return true;
}

bool base_parser::parse_custom_header(int32_t& character)
{
    parse_word(character, header_key_, &is_valid_token_character, lexer_);
    if (character != ':') {
        return false;
    }
    character = lexer_.get();
    parse_word(character, header_value_, &is_valid_header_value_character,
               lexer_);
    if (character != '\n') {
        return false;
    }

    for (size_t i = 0; i < header_key_.size(); i++) {
        header_key_[i] = to_lower(header_key_[i]);
    }

    auto it = headers_.find(header_key_.c_str());
    if (it == headers_.end()) {
        headers_[header_key_.c_str()] = header_value_.c_str();
    } else {
        it->second += std::string(",") + header_value_.c_str();
    }

    header_key_.clear();
    header_value_.clear();
    return true;
}

} // namespace http

} // namespace hutzn