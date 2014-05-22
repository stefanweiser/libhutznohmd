/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <cstdint>
#include <limits>
#include <map>

#include <http/parser/utility/date.hpp>
#include <http/parser/utility/trie.hpp>

#include "baseparser.hpp"

namespace rest
{

namespace http
{

base_parser::base_parser(const anonymous_int_function & get_functor,
                         const anonymous_int_function & peek_functor)
    : lexer_(get_functor, peek_functor)
    , state_(parser_state::UNFINISHED)
    , header_key_()
    , header_value_()
    , version_(rest::http::version::HTTP_UNKNOWN)
    , headers_()
    , content_length_(0)
    , content_type_(lexer_)
    , date_(time(NULL))
    , connection_(connection_type::KEEP_ALIVE)
{}

base_parser::~base_parser()
{}

bool base_parser::valid() const
{
    return (parser_state::SUCCEEDED == state_);
}

const rest::http::version & base_parser::version() const
{
    return version_;
}

const std::string base_parser::header(const std::string & key) const
{
    auto it = headers_.find(key);
    if (it != headers_.end()) {
        return it->second;
    }
    return "";
}

const size_t & base_parser::content_length() const
{
    return content_length_;
}

const time_t & base_parser::date() const
{
    return date_;
}

bool base_parser::is_keep_connection() const
{
    return (connection_ == connection_type::KEEP_ALIVE);
}

bool base_parser::parse_connection(int32_t & result)
{
    using value_info = trie<connection_type>::value_info;
    static const std::vector<value_info> types = {{
            value_info{"close", connection_type::CLOSE},
            value_info{"keep-alive", connection_type::KEEP_ALIVE},
            value_info{"persist", connection_type::KEEP_ALIVE}
        }
    };

    static const trie<connection_type> t(types, connection_type::ERROR);
    push_back_string<32> tmp;
    connection_type connection = t.parse(result, tmp, lexer_);
    if (connection != connection_type::ERROR) {
        connection_ = connection;
        return true;
    }
    return false;
}

bool base_parser::parse_content_length(int32_t & result)
{
    int32_t length = lexer_.get_unsigned_integer(result);
    if (length < 0) {
        return false;
    }

    content_length_ = static_cast<size_t>(length);
    return true;
}

bool base_parser::parse_content_type(int32_t & result)
{
    return content_type_.parse(result);
}

bool base_parser::parse_date(int32_t & result)
{
    time_t d = parse_timestamp(result, lexer_);
    if (d < 0) {
        return false;
    }

    date_ = d;
    return true;
}

bool base_parser::parse_header(int32_t & result)
{
    using parsing_function = bool (base_parser::*)(int32_t &);
    using trie_value = std::tuple<parsing_function, size_t>;
    using value_info = trie<trie_value>::value_info;
    static const std::vector<value_info> types = {{
            value_info{"content-length", trie_value{&base_parser::parse_content_length, 0}},
            value_info{"content-type", trie_value{&base_parser::parse_content_type, 1}},
            value_info{"date", trie_value{&base_parser::parse_date, 2}},
            value_info{"connection", trie_value{&base_parser::parse_connection, 3}}
        }
    };
    static const trie<trie_value> t(types, trie_value {nullptr, -1});
    trie_value value = t.parse(result, header_key_, lexer_);
    if (result == ':') {
        result = lexer_.get_non_whitespace();
        parsing_function functor = std::get<0>(value);
        if (nullptr != functor) {
            return (this->*functor)(result);
        }
    }

    size_t index = std::get<1>(value);
    if (index < types.size()) {
        header_key_.push_back(std::get<0>(types[index]));
    }

    parse_word(result, header_key_, &is_valid_token_character, lexer_);
    if (result != ':') {
        return false;
    }
    result = lexer_.get();
    parse_word(result, header_value_, &is_valid_header_value_character,
               lexer_);
    if (result != '\n') {
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

bool base_parser::parse_headers(int32_t & result)
{
    while (result != '\n') {
        if (false == parse_header(result)) {
            return false;
        }
        result = lexer_.get();
    }

    return true;
}

} // namespace http

} // namespace rest
