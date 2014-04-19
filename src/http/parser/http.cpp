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

bool parse_header_key_compared_to_string(int32_t & character,
        const std::string & already_parsed_string,
        const std::string & string,
        httpscan * scanner)
{
    size_t i = 0;
    size_t j = 0;
    character = scanner->lexer_.get();
    while ((character >= 0) &&
           (true == is_valid_header_key_character(static_cast<char>(character)))) {
        char c = static_cast<char>(character);
        if ((i == j) && ((i >= string.size()) || (true == compare_case_insensitive(string[i], c)))) {
            i++;
        } else {
            if (true == scanner->header_key_.empty()) {
                for (size_t k = 0; k < already_parsed_string.size(); k++) {
                    scanner->header_key_.push_back(already_parsed_string[k]);
                }
            }
            scanner->header_key_.push_back(c);
        }
        j++;
        character = scanner->lexer_.get();
    }

    return (i == j) && (i == string.size());
}

bool parse_header_type(int32_t & result, httpscan * scanner)
{
    do {
        if ((result < 0) || (false == is_valid_header_key_character(static_cast<uint8_t>(result)))) {
            result = -1;
            return false;
        }
        scanner->header_key_.push_back(to_lower(static_cast<char>(result)));
        result = scanner->lexer_.get();
    } while (result != ':');

    return true;
}

bool parse_header_value(int32_t & result, httpscan * scanner)
{
    while (result != '\n') {
        if ((result < 0) ||
            (false == is_valid_header_value_character(static_cast<uint8_t>(result)))) {
            return false;
        }
        scanner->header_value_.push_back(static_cast<char>(result));
        result = scanner->lexer_.get();
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

bool parse_header(int32_t & result, httpscan * scanner)
{
    /*if ((result == 'a') || (result == 'A')) {
        ;
    } else */if ((result == 'c') || (result == 'C')) {
        bool equal = parse_header_key_compared_to_string(result, "c", "ontent-length", scanner);
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
        bool equal = parse_header_key_compared_to_string(result, "d", "ate", scanner);
        if ((true == equal) && (result == ':')) {
            result = scanner->lexer_.get_non_whitespace();
            time_t date = parse_timestamp(result, scanner->lexer_);
            if (date == std::numeric_limits<time_t>::min()) {
                return false;
            }

            scanner->date_ = date;
            return true;
        }
    }/* else if ((result == 'e') || (result == 'E')) {
        ;
    } else if ((result == 'f') || (result == 'F')) {
        ;
    } else if ((result == 'h') || (result == 'H')) {
        ;
    } else if ((result == 'i') || (result == 'I')) {
        ;
    } else if ((result == 'l') || (result == 'L')) {
        ;
    } else if ((result == 'm') || (result == 'M')) {
        ;
    } else if ((result == 'p') || (result == 'P')) {
        ;
    } else if ((result == 'r') || (result == 'R')) {
        ;
    } else if ((result == 's') || (result == 'S')) {
        ;
    } else if ((result == 'u') || (result == 'U')) {
        ;
    } else if ((result == 'v') || (result == 'V')) {
        ;
    } else if ((result == 'w') || (result == 'W')) {
        bool equal = parse_header_key_compared_to_string(result, "w", "ww-authenticate", scanner);
        if ((true == equal) && (result == ':')) {
            return rest::http::header_type::WWW_AUTHENTICATE;
        }
    }*/

    parse_header_type(result, scanner);
    result = scanner->lexer_.get();
    if (false == parse_header_value(result, scanner)) {
        return false;
    }
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
