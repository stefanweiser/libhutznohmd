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

#include "responseparser.hpp"

namespace rest
{

namespace http
{

response_parser::response_parser(const anonymous_int_function & get_functor,
                                 const anonymous_int_function & peek_functor)
    : base_parser(get_functor, peek_functor)
    , status_code_(0)
    , reason_phrase_()
{}

namespace
{

uint16_t lex_status_code(int32_t & result, const lexer & l)
{
    int32_t code = l.get_unsigned_integer(result);
    if (code <= 0) {
        return 0;
    }
    return static_cast<uint16_t>(code);
}

bool lex_reason_phrase(int32_t & character, push_back_string<100> & phrase, const lexer & l)
{
    do {
        if ((character < 0) ||
            ((character != ' ') && (character != '\t') && (0 == isalnum(character)))) {
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
    if (parser_state::UNFINISHED != state_) {
        return;
    }

    int32_t result = lexer_.get_non_whitespace();
    {
        using value_info = trie<rest::http::version>::value_info;
        static const std::vector<value_info> types = {{
                value_info{"http/1.0", rest::http::version::HTTP_1_0},
                value_info{"http/1.1", rest::http::version::HTTP_1_1}
            }
        };

        static const trie<rest::http::version> t(types, rest::http::version::HTTP_UNKNOWN);
        push_back_string<32> tmp;
        version_ = t.parse(result, tmp, lexer_);
    }

    if (rest::http::version::HTTP_UNKNOWN == version_) {
        state_ = parser_state::ERROR;
        return;
    }

    result = lexer_.get_non_whitespace();
    status_code_ = lex_status_code(result, lexer_);
    if (0 == status_code_) {
        state_ = parser_state::ERROR;
        return;
    }

    result = lexer_.get_non_whitespace();
    if (false == lex_reason_phrase(result, reason_phrase_, lexer_)) {
        state_ = parser_state::ERROR;
        return;
    }

    result = lexer_.get();
    if (false == parse_headers(result)) {
        state_ = parser_state::ERROR;
        return;
    }
    state_ = parser_state::SUCCEEDED;
}

const uint16_t & response_parser::status_code() const
{
    return status_code_;
}

const char * response_parser::reason_phrase() const
{
    return reason_phrase_.c_str();
}

bool response_parser::parse_headers(int32_t & character)
{
    using trie_value_ = trie_value<response_parser>;
    using value_info = trie<trie_value_>::value_info;
    static const std::vector<value_info> types = {{
            value_info{"content-length", trie_value_{&response_parser::parse_content_length, 0}},
            value_info{"content-md5", trie_value_{&response_parser::parse_content_md5, 1}},
            value_info{"content-type", trie_value_{&response_parser::parse_content_type, 2}},
            value_info{"date", trie_value_{&response_parser::parse_date, 3}},
            value_info{"connection", trie_value_{&response_parser::parse_connection, 4}}
        }
    };

    while (character != '\n') {
        if (false == parse_generic_header<response_parser>(types, character)) {
            return false;
        }
        character = lexer_.get();
    }
    return true;
}

} // namespace http

} // namespace rest
