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

#include <http/parser/utility/charactercompare.hpp>

#include "httpfirstline.hpp"

namespace rest
{

namespace http
{

LOWER_CASE_STRING(ad);
LOWER_CASE_STRING(t);
LOWER_CASE_STRING(st);
LOWER_CASE_STRING(lete);
LOWER_CASE_STRING(tions);
LOWER_CASE_STRING(nnect);
LOWER_CASE_STRING(ace);

method lex_request_method(const int32_t & last, int32_t & character, const lexer & l)
{
    method result = method::UNKNOWN;
    if ((true == compare_case_insensitive('h', last)) &&
        (true == compare_case_insensitive('e', character))) {
        if (true == verify_forced_characters(lower_case_string_ad(), l)) {
            result = method::HEAD;
        }
    } else if ((true == compare_case_insensitive('g', last)) &&
               (true == compare_case_insensitive('e', character))) {
        if (true == verify_forced_characters(lower_case_string_t(), l)) {
            result = rest::http::method::GET;
        }
    } else if ((true == compare_case_insensitive('p', last))) {
        if (true == compare_case_insensitive('u', character)) {
            if (true == verify_forced_characters(lower_case_string_t(), l)) {
                result = rest::http::method::PUT;
            }
        } else if (true == compare_case_insensitive('o', character)) {
            if (true == verify_forced_characters(lower_case_string_st(), l)) {
                result = rest::http::method::POST;
            }
        }
    } else if ((true == compare_case_insensitive('d', last)) &&
               (true == compare_case_insensitive('e', character))) {
        if (true == verify_forced_characters(lower_case_string_lete(), l)) {
            result = rest::http::method::DELETE;
        }
    } else if ((true == compare_case_insensitive('o', last)) &&
               (true == compare_case_insensitive('p', character))) {
        if (true == verify_forced_characters(lower_case_string_tions(), l)) {
            result = rest::http::method::OPTIONS;
        }
    } else if ((true == compare_case_insensitive('c', last)) &&
               (true == compare_case_insensitive('o', character))) {
        if (true == verify_forced_characters(lower_case_string_nnect(), l)) {
            result = rest::http::method::CONNECT;
        }
    } else if ((true == compare_case_insensitive('t', last)) &&
               (true == compare_case_insensitive('r', character))) {
        if (true == verify_forced_characters(lower_case_string_ace(), l)) {
            result = rest::http::method::TRACE;
        }
    }

    return result;
}

bool lex_request_url(int32_t & character, push_back_string<1000> & url, const lexer & l)
{
    do {
        if ((character < 0) || (false == is_valid_url_character(static_cast<uint8_t>(character)))) {
            return false;
        }
        url.push_back(static_cast<char>(character));
        character = l.get();
    } while ((character != ' ') && (character != '\t'));
    return true;
}

LOWER_CASE_STRING(tp);

version lex_http_version(const int32_t & last, int32_t & character, const lexer & l)
{
    if ((true == compare_case_insensitive('h', last)) &&
        (true == compare_case_insensitive('t', character))) {
        if (false == verify_forced_characters(lower_case_string_tp(), l)) {
            return version::HTTP_UNKNOWN;
        }
        const int32_t slash = l.get();
        const int32_t one = l.get();
        const int32_t dot = l.get();
        if ((slash != '/') || (one != '1') || (dot != '.')) {
            return version::HTTP_UNKNOWN;
        }
        const int32_t version_digit = l.get();
        if (version_digit == '0') {
            return rest::http::version::HTTP_1_0;
        } else if (version_digit == '1') {
            return rest::http::version::HTTP_1_1;
        }
    }
    return version::HTTP_UNKNOWN;
}

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

} // namespace http

} // namespace rest
