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

#include <http/parser/utility/timestamp.hpp>
#include <http/parser/utility/trie.hpp>

#include "response_parser.hpp"

namespace hutzn
{

namespace http
{

response_parser::response_parser(const anonymous_int_function& get_functor,
                                 const anonymous_int_function& peek_functor)
    : common_(get_functor, peek_functor)
    , status_code_(0)
    , reason_phrase_()
{
}

namespace
{

uint16_t lex_status_code(int32_t& result, const lexer& l)
{
    int32_t code = l.get_unsigned_integer(result);
    if (code <= 0) {
        return 0;
    }
    return static_cast<uint16_t>(code);
}

bool lex_reason_phrase(int32_t& character, push_back_string<100>& phrase,
                       const lexer& l)
{
    do {
        if ((character < 0) || ((character != ' ') && (character != '\t') &&
                                (0 == isalnum(character)))) {
            return false;
        }
        phrase.push_back(static_cast<char>(character));
        character = l.get();
    } while (character != '\n');
    return true;
}

} // namespace

void response_parser::parse()
{
    if (parser_state::UNFINISHED != common_.state_) {
        return;
    }

    int32_t result = common_.lexer_.get_non_whitespace();
    {
        using value_info = trie<http::version>::value_info;
        static const std::vector<value_info> types = {
            {value_info{"http/1.0", http::version::HTTP_1_0},
             value_info{"http/1.1", http::version::HTTP_1_1}}};

        static const trie<http::version> t(types, http::version::HTTP_UNKNOWN);
        push_back_string<32> tmp;
        common_.version_ = t.parse(result, tmp, common_.lexer_);
    }

    if (http::version::HTTP_UNKNOWN == common_.version_) {
        common_.state_ = parser_state::ERROR;
        return;
    }

    result = common_.lexer_.get_non_whitespace();
    status_code_ = lex_status_code(result, common_.lexer_);
    if (0 == status_code_) {
        common_.state_ = parser_state::ERROR;
        return;
    }

    result = common_.lexer_.get_non_whitespace();
    if (false == lex_reason_phrase(result, reason_phrase_, common_.lexer_)) {
        common_.state_ = parser_state::ERROR;
        return;
    }

    result = common_.lexer_.get();
    if (false == parse_headers(result)) {
        common_.state_ = parser_state::ERROR;
        return;
    }
    common_.state_ = parser_state::SUCCEEDED;
}

bool response_parser::valid() const
{
    return (parser_state::SUCCEEDED == common_.state_);
}

const http::version& response_parser::version() const
{
    return common_.version_;
}

const std::map<std::string, std::string>& response_parser::headers() const
{
    return common_.headers_;
}

const size_t& response_parser::content_length() const
{
    return common_.content_length_;
}

const media_type_interface& response_parser::content_type() const
{
    return common_.content_type_;
}

const time_t& response_parser::date() const
{
    return common_.date_;
}

bool response_parser::keeps_connection() const
{
    return (common_.connection_ == connection_type::KEEP_ALIVE);
}

const std::array<uint8_t, 16>& response_parser::md5() const
{
    return common_.md5_;
}

bool response_parser::has_md5() const
{
    return common_.has_md5_;
}

const uint16_t& response_parser::status_code() const
{
    return status_code_;
}

const char* response_parser::reason_phrase() const
{
    return reason_phrase_.c_str();
}

bool response_parser::parse_connection(int32_t& character)
{
    return common_.parse_connection(character);
}

bool response_parser::parse_content_length(int32_t& character)
{
    return common_.parse_content_length(character);
}

bool response_parser::parse_content_md5(int32_t& character)
{
    return common_.parse_content_md5(character);
}

bool response_parser::parse_content_type(int32_t& character)
{
    return common_.parse_content_type(character);
}

bool response_parser::parse_date(int32_t& character)
{
    return common_.parse_date(character);
}

bool response_parser::parse_headers(int32_t& character)
{
    using trie_value_ = base_parser::trie_value<response_parser>;
    using value_info = trie<trie_value_>::value_info;
    static const std::vector<value_info> types = {
        {value_info{"content-length",
                    trie_value_{&response_parser::parse_content_length, 0}},
         value_info{"content-md5",
                    trie_value_{&response_parser::parse_content_md5, 1}},
         value_info{"content-type",
                    trie_value_{&response_parser::parse_content_type, 2}},
         value_info{"date", trie_value_{&response_parser::parse_date, 3}},
         value_info{"connection",
                    trie_value_{&response_parser::parse_connection, 4}}}};

    while (character != '\n') {
        if (false == common_.parse_generic_header(types, *this, character)) {
            return false;
        }
        character = common_.lexer_.get();
    }
    return true;
}

} // namespace http

} // namespace hutzn
