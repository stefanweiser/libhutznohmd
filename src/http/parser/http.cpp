/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <limits>
#include <iostream>
#include <sstream>

#include <http/parser/utility/charactercompare.hpp>
#include <http/parser/utility/httpdate.hpp>
#include <http/parser/utility/httpfirstline.hpp>
#include <http/parser/utility/trie.hpp>

#include "http.hpp"

namespace rest
{

namespace http
{

bool parse_content_length(int32_t & result, httpscan * scanner)
{
    int32_t code = scanner->lexer_.get_unsigned_integer(result);
    if (code < 0) {
        return false;
    }

    scanner->content_length_ = static_cast<size_t>(code);
    return true;
}

bool parse_content_type(int32_t & result, httpscan * scanner)
{
    return scanner->content_type_.parse(result);
}

bool parse_date(int32_t & result, httpscan * scanner)
{
    time_t date = parse_timestamp(result, scanner->lexer_);
    if (date < 0) {
        return false;
    }

    scanner->date_ = date;
    return true;
}

bool parse_header(int32_t & result, httpscan * scanner)
{
    using parsing_function = bool ( *)(int32_t &, httpscan *);
    using trie_value = std::tuple<parsing_function, size_t>;
    using value_info = trie<trie_value>::value_info;
    static const std::vector<value_info> types = {{
            value_info{"content-length", trie_value{&parse_content_length, 0}},
            value_info{"content-type", trie_value{&parse_content_type, 1}},
            value_info{"date", trie_value{&parse_date, 2}}
        }
    };
    static const trie<trie_value> t(types, trie_value {nullptr, -1});
    trie_value value = t.parse(result, scanner->header_key_, scanner->lexer_);
    if (result == ':') {
        result = scanner->lexer_.get_non_whitespace();
        parsing_function functor = std::get<0>(value);
        if (nullptr != functor) {
            return functor(result, scanner);
        }
    }

    size_t index = std::get<1>(value);
    if (index < types.size()) {
        scanner->header_key_.push_back(std::get<0>(types[index]));
    }

    parse_word(result, scanner->header_key_, &to_lower, &is_valid_token_character, scanner->lexer_);
    if (result != ':') {
        return false;
    }
    result = scanner->lexer_.get();
    parse_word(result, scanner->header_value_, &do_nothing, &is_valid_header_value_character,
               scanner->lexer_);
    if (result != '\n') {
        return false;
    }

    auto it = scanner->headers_.find(scanner->header_key_.c_str());
    if (it == scanner->headers_.end()) {
        scanner->headers_[scanner->header_key_.c_str()] = scanner->header_value_.c_str();
    } else {
        it->second += std::string(",") + scanner->header_value_.c_str();
    }

    scanner->header_key_.clear();
    scanner->header_value_.clear();
    return true;
}

bool parse_headers(int32_t & result, httpscan * scanner)
{
    while (result != '\n') {
        if (false == parse_header(result, scanner)) {
            return false;
        }
        result = scanner->lexer_.get();
    }

    return true;
}

parser_state lex_first_line(httpscan * scanner)
{
    int32_t result = scanner->lexer_.get_non_whitespace();
    {
        using value_type = std::tuple<method, version>;
        using value_info = trie<value_type>::value_info;
        static const std::vector<value_info> types = {{
                value_info{"head", value_type{method::HEAD, version::HTTP_UNKNOWN}},
                value_info{"get", value_type{method::GET, version::HTTP_UNKNOWN}},
                value_info{"put", value_type{method::PUT, version::HTTP_UNKNOWN}},
                value_info{"delete", value_type{method::DELETE, version::HTTP_UNKNOWN}},
                value_info{"post", value_type{method::POST, version::HTTP_UNKNOWN}},
                value_info{"trace", value_type{method::TRACE, version::HTTP_UNKNOWN}},
                value_info{"options", value_type{method::OPTIONS, version::HTTP_UNKNOWN}},
                value_info{"connect", value_type{method::CONNECT, version::HTTP_UNKNOWN}},
                value_info{"http/1.0", value_type{method::UNKNOWN, version::HTTP_1_0}},
                value_info{"http/1.1", value_type{method::UNKNOWN, version::HTTP_1_1}}
            }
        };

        static const trie<value_type> t(types, value_type {method::UNKNOWN, version::HTTP_UNKNOWN});
        push_back_string<32> tmp;
        std::tie(scanner->method_, scanner->version_) = t.parse(result, tmp, scanner->lexer_);
    }

    if (version::HTTP_UNKNOWN != scanner->version_) {
        result = scanner->lexer_.get_non_whitespace();
        scanner->status_code_ = lex_status_code(result, scanner->lexer_);
        if (0 == scanner->status_code_) {
            return parser_state::ERROR;
        }
        result = scanner->lexer_.get_non_whitespace();
        if (false == lex_reason_phrase(result, scanner->reason_phrase_, scanner->lexer_)) {
            return parser_state::ERROR;
        }
    } else {
        if (method::UNKNOWN == scanner->method_) {
            return parser_state::ERROR;
        }
        result = scanner->lexer_.get_non_whitespace();
        if (false == lex_request_url(result, scanner->url_, scanner->lexer_)) {
            return parser_state::ERROR;
        }
        result = scanner->lexer_.get_non_whitespace();

        {
            using value_info = trie<version>::value_info;
            static const std::vector<value_info> types = {{
                    value_info{"http/1.0", version::HTTP_1_0},
                    value_info{"http/1.1", version::HTTP_1_1}
                }
            };

            static const trie<version> t(types, version::HTTP_UNKNOWN);
            push_back_string<32> tmp;
            scanner->version_ = t.parse(result, tmp, scanner->lexer_);
        }

        if (version::HTTP_UNKNOWN == scanner->version_) {
            return parser_state::ERROR;
        }
        if (result != '\n') {
            return parser_state::ERROR;
        }
    }

    result = scanner->lexer_.get();
    if (false == parse_headers(result, scanner)) {
        return parser_state::ERROR;
    }
    return parser_state::SUCCEEDED;
}

void httpscan::parse()
{
    if ((state_ != parser_state::SUCCEEDED) &&
        (state_ != parser_state::ERROR)) {
        state_ = lex_first_line(this);
    }
}

} // namespace http

} // namespace rest
