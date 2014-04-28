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

version lex_http_version(int32_t & character, const lexer & l)
{
    character = l.get();
    if (character == '0') {
        return rest::http::version::HTTP_1_0;
    } else if (character == '1') {
        return rest::http::version::HTTP_1_1;
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
