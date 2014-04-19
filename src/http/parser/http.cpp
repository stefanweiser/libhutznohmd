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

LOWER_CASE_STRING(ad);
LOWER_CASE_STRING(t);
LOWER_CASE_STRING(st);
LOWER_CASE_STRING(lete);
LOWER_CASE_STRING(tions);
LOWER_CASE_STRING(nnect);
LOWER_CASE_STRING(ace);

bool lex_request_method(const int32_t & last, int32_t & result, httpscan * scanner)
{
    bool succeeded = true;
    if ((true == compare_case_insensitive('h', last)) &&
        (true == compare_case_insensitive('e', result))) {
        scanner->method_ = rest::http::method::HEAD;
        succeeded = verify_forced_characters(lower_case_string_ad(), scanner->lexer_);
    } else if ((true == compare_case_insensitive('g', last)) &&
               (true == compare_case_insensitive('e', result))) {
        scanner->method_ = rest::http::method::GET;
        succeeded = verify_forced_characters(lower_case_string_t(), scanner->lexer_);
    } else if ((true == compare_case_insensitive('p', last))) {
        if (true == compare_case_insensitive('u', result)) {
            scanner->method_ = rest::http::method::PUT;
            succeeded = verify_forced_characters(lower_case_string_t(), scanner->lexer_);
        } else if (true == compare_case_insensitive('o', result)) {
            scanner->method_ = rest::http::method::POST;
            succeeded = verify_forced_characters(lower_case_string_st(), scanner->lexer_);
        }
    } else if ((true == compare_case_insensitive('d', last)) &&
               (true == compare_case_insensitive('e', result))) {
        scanner->method_ = rest::http::method::DELETE;
        succeeded = verify_forced_characters(lower_case_string_lete(), scanner->lexer_);
    } else if ((true == compare_case_insensitive('o', last)) &&
               (true == compare_case_insensitive('p', result))) {
        scanner->method_ = rest::http::method::OPTIONS;
        succeeded = verify_forced_characters(lower_case_string_tions(), scanner->lexer_);
    } else if ((true == compare_case_insensitive('c', last)) &&
               (true == compare_case_insensitive('o', result))) {
        scanner->method_ = rest::http::method::CONNECT;
        succeeded = verify_forced_characters(lower_case_string_nnect(), scanner->lexer_);
    } else if ((true == compare_case_insensitive('t', last)) &&
               (true == compare_case_insensitive('r', result))) {
        scanner->method_ = rest::http::method::TRACE;
        succeeded = verify_forced_characters(lower_case_string_ace(), scanner->lexer_);
    } else {
        succeeded = false;
    }

    return succeeded;
}

bool lex_request_url(int32_t & result, httpscan * scanner)
{
    int32_t character = result;
    do {
        if ((character < 0) || (false == is_valid_url_character(static_cast<uint8_t>(character)))) {
            return false;
        }
        scanner->url_.push_back(static_cast<char>(character));
        character = scanner->lexer_.get();
    } while ((character != ' ') && (character != '\t'));
    return true;
}

LOWER_CASE_STRING(tp);

bool lex_http_version(const int32_t & last, int32_t & result, httpscan * scanner)
{
    bool succeeded = true;
    if ((true == compare_case_insensitive('h', last)) &&
        (true == compare_case_insensitive('t', result))) {
        succeeded = verify_forced_characters(lower_case_string_tp(), scanner->lexer_);
        const int32_t slash = scanner->lexer_.get();
        const int32_t one = scanner->lexer_.get();
        const int32_t dot = scanner->lexer_.get();
        if ((slash != '/') || (one != '1') || (dot != '.')) {
            succeeded = false;
        }
        const int32_t version_digit = scanner->lexer_.get();
        if (version_digit == '0') {
            scanner->version_ = rest::http::version::HTTP_1_0;
        } else if (version_digit == '1') {
            scanner->version_ = rest::http::version::HTTP_1_1;
        } else {
            succeeded = false;
        }
    } else {
        succeeded = false;
    }

    return succeeded;
}

bool lex_status_code(int32_t & result, httpscan * scanner)
{
    int32_t code = scanner->lexer_.get_unsigned_integer(result);
    if (code < 0) {
        return false;
    }
    scanner->status_code_ = static_cast<uint16_t>(code);
    return true;
}

bool lex_reason_phrase(int32_t & result, httpscan * scanner)
{
    int32_t character = result;
    do {
        if ((character < 0) ||
            ((character != ' ') && (character != '\t') && (0 == isalnum(character)))) {
            return false;
        }
        scanner->reason_phrase_.push_back(static_cast<char>(character));
        character = scanner->lexer_.get();
    } while (character != '\n');
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
        if (false == lex_http_version(last, result, scanner)) {
            return parser_state::ERROR;
        }
        result = scanner->lexer_.get_non_whitespace();
        if (false == lex_status_code(result, scanner)) {
            return parser_state::ERROR;
        }
        result = scanner->lexer_.get_non_whitespace();
        if (false == lex_reason_phrase(result, scanner)) {
            return parser_state::ERROR;
        }
    } else {
        if (false == lex_request_method(last, result, scanner)) {
            return parser_state::ERROR;
        }
        result = scanner->lexer_.get_non_whitespace();
        if (false == lex_request_url(result, scanner)) {
            return parser_state::ERROR;
        }
        last = scanner->lexer_.get_non_whitespace();
        result = scanner->lexer_.get();
        if (false == lex_http_version(last, result, scanner)) {
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

void http_parse(httpscan * scanner)
{
    if ((scanner->state_ != parser_state::SUCCEEDED) &&
        (scanner->state_ != parser_state::ERROR)) {
        scanner->state_ = lex_first_line(scanner);
    }
}

} // namespace http

} // namespace rest
