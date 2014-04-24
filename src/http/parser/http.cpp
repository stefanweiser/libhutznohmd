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
#include <sstream>

#include <http/parser/utility/charactercompare.hpp>
#include <http/parser/utility/httpdate.hpp>
#include <http/parser/utility/httpfirstline.hpp>

#include "http.hpp"

namespace rest
{

namespace http
{

bool parse_header(int32_t & result, httpscan * scanner)
{
    if ((result == 'c') || (result == 'C')) {
        bool equal = compare_to_reference(result, "content-length", 1, scanner->header_key_,
                                          &is_valid_token_character, scanner->lexer_);
        if ((true == equal) && (result == ':')) {
            result = scanner->lexer_.get_non_whitespace();
            int32_t code = scanner->lexer_.get_unsigned_integer(result);
            if (code < 0) {
                return false;
            }

            scanner->content_length_ = static_cast<size_t>(code);
            return true;
        }
    } else if ((result == 'd') || (result == 'D')) {
        bool equal = compare_to_reference(result, "date", 1, scanner->header_key_,
                                          &is_valid_token_character, scanner->lexer_);
        if ((true == equal) && (result == ':')) {
            result = scanner->lexer_.get_non_whitespace();
            time_t date = parse_timestamp(result, scanner->lexer_);
            if (date < 0) {
                return false;
            }

            scanner->date_ = date;
            return true;
        }
    }

    parse_word(result, scanner->header_key_, &to_lower, &is_valid_token_character, scanner->lexer_);
    if (result != ':') {
        return false;
    }
    result = scanner->lexer_.get();
    auto nothing = [](const char & c) {
        return c;
    };
    parse_word(result, scanner->header_value_, nothing, &is_valid_header_value_character,
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
    int32_t last = scanner->lexer_.get_non_whitespace();
    int32_t result = scanner->lexer_.get();
    if ((last < 0) || (result < 0)) {
        return parser_state::ERROR;
    }

    if ((true == compare_case_insensitive('h', last)) &&
        ((true == compare_case_insensitive('t', result)))) {
        scanner->version_ = lex_http_version(last, result, scanner->lexer_);
        if (version::HTTP_UNKNOWN == scanner->version_) {
            return parser_state::ERROR;
        }
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
        scanner->method_ = lex_request_method(last, result, scanner->lexer_);
        if (method::UNKNOWN == scanner->method_) {
            return parser_state::ERROR;
        }
        result = scanner->lexer_.get_non_whitespace();
        if (false == lex_request_url(result, scanner->url_, scanner->lexer_)) {
            return parser_state::ERROR;
        }
        last = scanner->lexer_.get_non_whitespace();
        result = scanner->lexer_.get();
        scanner->version_ = lex_http_version(last, result, scanner->lexer_);
        if (version::HTTP_UNKNOWN == scanner->version_) {
            return parser_state::ERROR;
        }
        const int32_t newline = scanner->lexer_.get_non_whitespace();
        if (newline != '\n') {
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
