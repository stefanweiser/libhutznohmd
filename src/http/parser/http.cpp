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

#include <sstream>

#include <http/parser/utility/lower_case_string.hpp>

#include "http.hpp"

char to_lower(const char c)
{
    if ((c >= 'A') && (c <= 'Z')) {
        return (c | 0x60);
    }
    return c;
}

rest::http::header_type header_key_to_header_type(const std::string & s)
{
    const static std::map<std::string, rest::http::header_type> string_to_enum_map = {
        {"accept", rest::http::header_type::ACCEPT},
        {"accept-charset", rest::http::header_type::ACCEPT_CHARSET},
        {"accept-encoding", rest::http::header_type::ACCEPT_ENCODING},
        {"accept-language", rest::http::header_type::ACCEPT_LANGUAGE},
        {"accept-ranges", rest::http::header_type::ACCEPT_RANGES},
        {"age", rest::http::header_type::AGE},
        {"allow", rest::http::header_type::ALLOW},
        {"authorization", rest::http::header_type::AUTHORIZATION},
        {"cache-control", rest::http::header_type::CACHE_CONTROL},
        {"connection", rest::http::header_type::CONNECTION},
        {"content-encoding", rest::http::header_type::CONTENT_ENCODING},
        {"content-language", rest::http::header_type::CONTENT_LANGUAGE},
        {"content-length", rest::http::header_type::CONTENT_LENGTH},
        {"content-location", rest::http::header_type::CONTENT_LOCATION},
        {"content-md5", rest::http::header_type::CONTENT_MD5},
        {"content-range", rest::http::header_type::CONTENT_RANGE},
        {"content-type", rest::http::header_type::CONTENT_TYPE},
        {"cookie", rest::http::header_type::COOKIE},
        {"date", rest::http::header_type::DATE},
        {"etag", rest::http::header_type::ETAG},
        {"expect", rest::http::header_type::EXPECT},
        {"expires", rest::http::header_type::EXPIRES},
        {"from", rest::http::header_type::FROM},
        {"host", rest::http::header_type::HOST},
        {"if-match", rest::http::header_type::IF_MATCH},
        {"if-modified-since", rest::http::header_type::IF_MODIFIED_SINCE},
        {"if-none-match", rest::http::header_type::IF_NONE_MATCH},
        {"if-range", rest::http::header_type::IF_RANGE},
        {"if-unmodified-since", rest::http::header_type::IF_UNMODIFIED_SINCE},
        {"last-modified", rest::http::header_type::LAST_MODIFIED},
        {"location", rest::http::header_type::LOCATION},
        {"max-forwards", rest::http::header_type::MAX_FORWARDS},
        {"pragma", rest::http::header_type::PRAGMA},
        {"proxy-authenticate", rest::http::header_type::PROXY_AUTHENTICATE},
        {"proxy-authorization", rest::http::header_type::PROXY_AUTHORIZATION},
        {"range", rest::http::header_type::RANGE},
        {"referer", rest::http::header_type::REFERER},
        {"retry-after", rest::http::header_type::RETRY_AFTER},
        {"server", rest::http::header_type::SERVER},
        {"user-agent", rest::http::header_type::USER_AGENT},
        {"vary", rest::http::header_type::VARY},
        {"www-authenticate", rest::http::header_type::WWW_AUTHENTICATE}
    };

    auto it = string_to_enum_map.find(s);
    if (it != string_to_enum_map.end()) {
        return it->second;
    }
    return rest::http::header_type::CUSTOM;
}

bool is_valid_url_character(uint8_t c)
{
    static const std::array<char, 256> validity_map = {
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    };
    return (validity_map[c] != 0);
}

bool is_valid_header_key_character(uint8_t c)
{
    static const std::array<char, 256> validity_map = {
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    };
    return (validity_map[c] != 0);
}

bool is_valid_header_value_character(uint8_t c)
{
    static const std::array<char, 256> validity_map = {
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        }
    };
    return (validity_map[c] != 0);
}

inline int get_normalized_char(httpscan_t * scanner)
{
    if (lexer_state::FINISHED == scanner->state_) {
        return -1;
    }

    int result = scanner->get_functor_();
    if (result == '\r') {
        if (scanner->peek_functor_() == '\n') {
            scanner->get_functor_();
        }
        result = '\n';
    }

    if ((result == '\n') && (scanner->last_char_ != '\n')) {
        if ((scanner->peek_functor_() == ' ') || (scanner->peek_functor_() == '\t')) {
            scanner->get_functor_();
            result = ' ';
        }
    }

    scanner->last_char_ = static_cast<char>(result);
    return result;
}

inline int get_next_non_whitespace(httpscan_t * scanner)
{
    int result = get_normalized_char(scanner);
    while ((result == ' ') || (result == '\t')) {
        result = get_normalized_char(scanner);
    }
    return result;
}

bool compare_case_insensitive(const char lower_char, const char indetermined_letter)
{
    return ((indetermined_letter | 0x20) == lower_char);
}

template<typename lower_case_string>
bool verify_forced_characters(const lower_case_string &, httpscan_t * scanner)
{
    for (size_t i = 0; i < lower_case_string::size; i++) {
        const int character = get_normalized_char(scanner);
        if (false == compare_case_insensitive(lower_case_string::value[i], character)) {
            return false;
        }
    }
    return true;
}

bool parse_header_key_to_string(int & character,
                                const std::string & already_parsed_string,
                                const std::string & string,
                                httpscan_t * scanner)
{
    size_t i = 0;
    size_t j = 0;
    character = get_normalized_char(scanner);
    while ((character >= 0) &&
           (true == is_valid_header_key_character(static_cast<char>(character)))) {
        char c = static_cast<char>(character);
        if ((i == j) && ((i >= string.size()) || (string[i] == c) || (string[i] == to_lower(c)))) {
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
        character = get_normalized_char(scanner);
    }

    return (i == j);
}

bool parse_custom_header_type(int & result, httpscan_t * scanner)
{
    int character = result;
    do {
        if ((character < 0) || (false == is_valid_header_key_character(static_cast<uint8_t>(character)))) {
            result = -1;
            return false;
        }
        scanner->header_key_.push_back(to_lower(static_cast<char>(character)));
        character = get_normalized_char(scanner);
    } while (character != ':');

    return true;
}

/*
        {"accept", rest::http::header_type::ACCEPT},
        {"accept-charset", rest::http::header_type::ACCEPT_CHARSET},
        {"accept-encoding", rest::http::header_type::ACCEPT_ENCODING},
        {"accept-language", rest::http::header_type::ACCEPT_LANGUAGE},
        {"accept-ranges", rest::http::header_type::ACCEPT_RANGES},
        {"age", rest::http::header_type::AGE},
        {"allow", rest::http::header_type::ALLOW},
        {"authorization", rest::http::header_type::AUTHORIZATION},
        {"cache-control", rest::http::header_type::CACHE_CONTROL},
        {"connection", rest::http::header_type::CONNECTION},
        {"content-encoding", rest::http::header_type::CONTENT_ENCODING},
        {"content-language", rest::http::header_type::CONTENT_LANGUAGE},
        {"content-length", rest::http::header_type::CONTENT_LENGTH},
        {"content-location", rest::http::header_type::CONTENT_LOCATION},
        {"content-md5", rest::http::header_type::CONTENT_MD5},
        {"content-range", rest::http::header_type::CONTENT_RANGE},
        {"content-type", rest::http::header_type::CONTENT_TYPE},
        {"cookie", rest::http::header_type::COOKIE},
        {"date", rest::http::header_type::DATE},
        {"etag", rest::http::header_type::ETAG},
        {"expect", rest::http::header_type::EXPECT},
        {"expires", rest::http::header_type::EXPIRES},
        {"from", rest::http::header_type::FROM},
        {"host", rest::http::header_type::HOST},
        {"if-match", rest::http::header_type::IF_MATCH},
        {"if-modified-since", rest::http::header_type::IF_MODIFIED_SINCE},
        {"if-none-match", rest::http::header_type::IF_NONE_MATCH},
        {"if-range", rest::http::header_type::IF_RANGE},
        {"if-unmodified-since", rest::http::header_type::IF_UNMODIFIED_SINCE},
        {"last-modified", rest::http::header_type::LAST_MODIFIED},
        {"location", rest::http::header_type::LOCATION},
        {"max-forwards", rest::http::header_type::MAX_FORWARDS},
        {"pragma", rest::http::header_type::PRAGMA},
        {"proxy-authenticate", rest::http::header_type::PROXY_AUTHENTICATE},
        {"proxy-authorization", rest::http::header_type::PROXY_AUTHORIZATION},
        {"range", rest::http::header_type::RANGE},
        {"referer", rest::http::header_type::REFERER},
        {"retry-after", rest::http::header_type::RETRY_AFTER},
        {"server", rest::http::header_type::SERVER},
        {"user-agent", rest::http::header_type::USER_AGENT},
        {"vary", rest::http::header_type::VARY},
        {"www-authenticate", rest::http::header_type::WWW_AUTHENTICATE}
*/

rest::http::header_type parse_header_type(int & result, httpscan_t * scanner)
{
    /*if ((result == 'a') || (result == 'A')) {
        ;
    } else if ((result == 'c') || (result == 'C')) {
        bool equal = parse_header_key_to_string(result, "c", "ontent-length", scanner);
        if ((true == equal) && (result == ':')) {
            return rest::http::header_type::CONTENT_LENGTH;
        }
    } else if ((result == 'd') || (result == 'D')) {
        ;
    } else if ((result == 'e') || (result == 'E')) {
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
    } else*/ if ((result == 'w') || (result == 'W')) {
        bool equal = parse_header_key_to_string(result, "w", "ww-authenticate", scanner);
        if ((true == equal) && (result == ':')) {
            return rest::http::header_type::WWW_AUTHENTICATE;
        }
    } else {
        parse_custom_header_type(result, scanner);
    }
    return header_key_to_header_type(scanner->header_key_.c_str());
//  return rest::http::header_type::CUSTOM;
}

LOWER_CASE_STRING(ead);
LOWER_CASE_STRING(et);
LOWER_CASE_STRING(t);
LOWER_CASE_STRING(st);
LOWER_CASE_STRING(elete);
LOWER_CASE_STRING(ptions);
LOWER_CASE_STRING(onnect);
LOWER_CASE_STRING(race);

lexer_state lex_request_method(int & result, httpscan_t * scanner)
{
    bool succeeded = true;
    if (('h' == result) || ('H' == result)) {
        scanner->method_ = rest::http::method::HEAD;
        succeeded = verify_forced_characters(lower_case_string_ead(), scanner);
    } else if (('g' == result) || ('G' == result)) {
        scanner->method_ = rest::http::method::GET;
        succeeded = verify_forced_characters(lower_case_string_et(), scanner);
    } else if (('p' == result) || ('P' == result)) {
        result = get_normalized_char(scanner);
        if (('u' == result) || ('U' == result)) {
            scanner->method_ = rest::http::method::PUT;
            succeeded = verify_forced_characters(lower_case_string_t(), scanner);
        } else if (('o' == result) || ('O' == result)) {
            scanner->method_ = rest::http::method::POST;
            succeeded = verify_forced_characters(lower_case_string_st(), scanner);
        }
    } else if (('d' == result) || ('D' == result)) {
        scanner->method_ = rest::http::method::DELETE;
        succeeded = verify_forced_characters(lower_case_string_elete(), scanner);
    } else if (('o' == result) || ('O' == result)) {
        scanner->method_ = rest::http::method::OPTIONS;
        succeeded = verify_forced_characters(lower_case_string_ptions(), scanner);
    } else if (('c' == result) || ('C' == result)) {
        scanner->method_ = rest::http::method::CONNECT;
        succeeded = verify_forced_characters(lower_case_string_onnect(), scanner);
    } else if (('t' == result) || ('T' == result)) {
        scanner->method_ = rest::http::method::TRACE;
        succeeded = verify_forced_characters(lower_case_string_race(), scanner);
    } else {
        succeeded = false;
    }

    lexer_state next_state = (true == succeeded) ? lexer_state::REQUEST_URL : lexer_state::ERROR;
    return next_state;
}

lexer_state lex_request_url(int & result, httpscan_t * scanner)
{
    int character = result;
    do {
        if ((character < 0) || (false == is_valid_url_character(static_cast<uint8_t>(character)))) {
            return lexer_state::ERROR;
        }
        scanner->url_.push_back(static_cast<char>(character));
        character = get_normalized_char(scanner);
    } while ((character != ' ') && (character != '\t'));
    return lexer_state::REQUEST_VERSION;
}

LOWER_CASE_STRING(ttp);

bool lex_http_version(int & result, httpscan_t * scanner)
{
    bool succeeded = true;
    if (('h' == result) || ('H' == result)) {
        succeeded = verify_forced_characters(lower_case_string_ttp(), scanner);
        const int slash = get_normalized_char(scanner);
        const int one = get_normalized_char(scanner);
        const int dot = get_normalized_char(scanner);
        if ((slash != '/') || (one != '1') || (dot != '.')) {
            succeeded = false;
        }
        const int version_digit = get_normalized_char(scanner);
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

int parse_unsigned_integer(int & digit, httpscan_t * scanner)
{
    if (0 == isdigit(digit)) {
        return -1;
    }

    int result = 0;
    do {
        if (digit < 0) {
            return -1;
        }
        result = (result * 10) + (digit - 0x30);
        digit = get_normalized_char(scanner);
    } while (0 != isdigit(digit));

    return result;
}

lexer_state lex_status_code(int & result, httpscan_t * scanner)
{
    int code = parse_unsigned_integer(result, scanner);
    if (code < 0) {
        return lexer_state::ERROR;
    }
    scanner->status_code_ = static_cast<uint16_t>(code);
    return lexer_state::RESPONSE_REASON_PHRASE;
}

lexer_state lex_reason_phrase(int & result, httpscan_t * scanner)
{
    int character = result;
    do {
        if ((character < 0) ||
            ((character != ' ') && (character != '\t') && (0 == isalnum(character)))) {
            return lexer_state::ERROR;
        }
        scanner->reason_phrase_.push_back(static_cast<char>(character));
        character = get_normalized_char(scanner);
    } while (character != '\n');
    return lexer_state::HEADER;
}

lexer_state lex_request_version(int & result, httpscan_t * scanner)
{
    if (true == lex_http_version(result, scanner)) {
        const int newline = get_normalized_char(scanner);
        if (newline == '\n') {
            return lexer_state::HEADER;
        }
    }
    return lexer_state::ERROR;
}

lexer_state lex_response_version(int & result, httpscan_t * scanner)
{
    if (true == lex_http_version(result, scanner)) {
        return lexer_state::RESPONSE_STATUS_CODE;
    }
    return lexer_state::ERROR;
}

lexer_state lex_header_value(int & result, rest::http::header_type type, httpscan_t * scanner)
{
    int character = result;
    while (character != '\n') {
        if ((character < 0) ||
            (false == is_valid_header_value_character(static_cast<uint8_t>(character)))) {
            return lexer_state::ERROR;
        }
        scanner->header_value_.push_back(static_cast<char>(character));
        character = get_normalized_char(scanner);
    }

    if (type == rest::http::header_type::CUSTOM) {
        auto it = scanner->custom_headers_.find(scanner->header_key_.c_str());
        if (it == scanner->custom_headers_.end()) {
            scanner->custom_headers_[scanner->header_key_.c_str()] = scanner->header_value_.c_str();
        } else {
            it->second += std::string(",") + scanner->header_value_.c_str();
        }
    } else {
        auto it = scanner->headers_.find(type);
        if (it == scanner->headers_.end()) {
            scanner->headers_[type] = scanner->header_value_.c_str();
        } else {
            it->second += std::string(",") + scanner->header_value_.c_str();
        }
    }

    scanner->header_key_.clear();
    scanner->header_value_.clear();

    return lexer_state::HEADER;
}

lexer_state lex_header(int & result, httpscan_t * scanner)
{
    if (result == '\n') {
        return lexer_state::FINISHED;
    }

    rest::http::header_type type = parse_header_type(result, scanner);
    if (result < 0) {
        return lexer_state::ERROR;
    }

    if (type == rest::http::header_type::CONTENT_LENGTH) {
        result = get_next_non_whitespace(scanner);
        int code = parse_unsigned_integer(result, scanner);
        if (code < 0) {
            return lexer_state::ERROR;
        }
        scanner->content_length_ = static_cast<size_t>(code);
        scanner->header_key_.clear();
        return lexer_state::HEADER;
    } else {
        result = get_normalized_char(scanner);
        return lex_header_value(result, type, scanner);
    }
}

int httplex(httpscan_t * scanner)
{
    if ((scanner->state_ == lexer_state::FINISHED) ||
        (scanner->state_ == lexer_state::ERROR)) {
        return -1;
    }

    int result = 0;
    if ((scanner->state_ != lexer_state::HEADER) &&
        (scanner->state_ != lexer_state::FINISHED) &&
        (scanner->state_ != lexer_state::ERROR)) {
        result = get_next_non_whitespace(scanner);
    } else {
        result = get_normalized_char(scanner);
    }
    if (result < 0) {
        if (lexer_state::FINISHED != scanner->state_) {
            scanner->state_ = lexer_state::ERROR;
        }
        return -1;
    }

    if (lexer_state::START == scanner->state_) {
        int peek_result = scanner->peek_functor_();
        if (((result == 'h') || (result == 'H')) &&
            ((peek_result == 't') || (peek_result == 'T'))) {
            scanner->state_ = lexer_state::RESPONSE_VERSION;
        } else {
            scanner->state_ = lexer_state::REQUEST_METHOD;
        }
    }

    switch (scanner->state_) {
    case lexer_state::REQUEST_METHOD:
        scanner->state_ = lex_request_method(result, scanner);
        break;

    case lexer_state::REQUEST_URL:
        scanner->state_ = lex_request_url(result, scanner);
        break;

    case lexer_state::REQUEST_VERSION:
        scanner->state_ = lex_request_version(result, scanner);
        break;

    case lexer_state::RESPONSE_VERSION:
        scanner->state_ = lex_response_version(result, scanner);
        break;

    case lexer_state::RESPONSE_STATUS_CODE:
        scanner->state_ = lex_status_code(result, scanner);
        break;

    case lexer_state::RESPONSE_REASON_PHRASE:
        scanner->state_ = lex_reason_phrase(result, scanner);
        break;

    case lexer_state::HEADER:
        scanner->state_ = lex_header(result, scanner);
        break;

    case lexer_state::START:
    case lexer_state::FINISHED:
    case lexer_state::ERROR:
    default:
        break;
    }

    return result;
}

void http_parse(httpscan_t * scanner)
{
    int result = 0;
    while (result >= 0) {
        result = httplex(scanner);
    }
}
