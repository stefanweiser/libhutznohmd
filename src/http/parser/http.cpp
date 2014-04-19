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

int get_normalized_char(httpscan_t * scanner)
{
    int result = scanner->get_functor_();
    if (result == '\r') {
        if (scanner->peek_functor_() == '\n') {
            scanner->get_functor_();
        }
        result = '\n';
    }

    if ((result == '\n') && (scanner->last_char_ != '\n')) {
        const int n = scanner->peek_functor_();
        if ((n == ' ') || (n == '\t')) {
            scanner->get_functor_();
            result = ' ';
        }
    }

    scanner->last_char_ = static_cast<char>(result);
    return result;
}

int get_next_non_whitespace(httpscan_t * scanner)
{
    int result = 0;
    do {
        result = get_normalized_char(scanner);
    } while ((result == ' ') || (result == '\t'));
    return result;
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

int parse_time(int & character, httpscan_t * scanner)
{
    int hour = parse_unsigned_integer(character, scanner);
    if ((hour < 0) || (hour > 23) || (character != ':')) {
        return -1;
    }
    character = get_next_non_whitespace(scanner);
    int minute = parse_unsigned_integer(character, scanner);
    if ((minute < 0) || (minute > 59) || (character != ':')) {
        return -1;
    }
    character = get_next_non_whitespace(scanner);
    int second = parse_unsigned_integer(character, scanner);
    if ((second < 0) || (second > 59)) {
        return -1;
    }
    return (60 * ((60 * hour) + minute)) + second;
}

int parse_month(const char c1, const char c2, const char c3)
{
    if (true == compare_case_insensitive('j', c1)) {
        if ((true == compare_case_insensitive('a', c2)) &&
            (true == compare_case_insensitive('n', c3))) {
            return 1;
        } else if (true == compare_case_insensitive('u', c2)) {
            if (true == compare_case_insensitive('n', c3)) {
                return 6;
            } else if (true == compare_case_insensitive('l', c3)) {
                return 7;
            }
        }
    } else if ((true == compare_case_insensitive('f', c1)) &&
               (true == compare_case_insensitive('e', c2)) &&
               (true == compare_case_insensitive('b', c3))) {
        return 2;
    } else if ((true == compare_case_insensitive('m', c1)) &&
               (true == compare_case_insensitive('a', c2))) {
        if (true == compare_case_insensitive('r', c3)) {
            return 3;
        } else if (true == compare_case_insensitive('y', c3)) {
            return 5;
        }
    } else if ((true == compare_case_insensitive('a', c1))) {
        if ((true == compare_case_insensitive('p', c2)) &&
            (true == compare_case_insensitive('r', c3))) {
            return 4;
        } else if ((true == compare_case_insensitive('u', c2)) &&
                   (true == compare_case_insensitive('g', c3))) {
            return 8;
        }
    } else if ((true == compare_case_insensitive('s', c1)) &&
               (true == compare_case_insensitive('e', c2)) &&
               (true == compare_case_insensitive('p', c3))) {
        return 9;
    } else if ((true == compare_case_insensitive('o', c1)) &&
               (true == compare_case_insensitive('c', c2)) &&
               (true == compare_case_insensitive('t', c3))) {
        return 10;
    } else if ((true == compare_case_insensitive('n', c1)) &&
               (true == compare_case_insensitive('o', c2)) &&
               (true == compare_case_insensitive('v', c3))) {
        return 11;
    } else if ((true == compare_case_insensitive('d', c1)) &&
               (true == compare_case_insensitive('e', c2)) &&
               (true == compare_case_insensitive('c', c3))) {
        return 12;
    }
    return -1;
}

time_t day_of_the_year(const time_t day,
                       const time_t month,
                       const time_t year)
{
    time_t result = day;
    if (month < 3) {
        result += (306 * month - 301) / 10;
    } else {
        result += (306 * month - 913) / 10;
        if ((year % 4) == 0) {
            result += 60;
        } else {
            result += 59;
        }
    }
    return result;
}

time_t seconds_since_epoch(const time_t second_of_day,
                           const time_t day,
                           const time_t month,
                           const time_t year)
{
    const time_t second_of_year = second_of_day +
                                  ((day_of_the_year(day, month, year) - 1) * 86400);
    const time_t year_seconds_since_epoch = ((year - 1970) * 86400 * 365) +
                                            (((year - (1972 - 3)) / 4) * 86400);

    return year_seconds_since_epoch + second_of_year;
}

LOWER_CASE_STRING(mt);

bool is_valid_epoch_date(const time_t day, const time_t month, const time_t year)
{
    if ((year < 1970) ||
        (month < 1) ||
        (month > 12) ||
        (day < 1) ||
        ((month < 8) && ((month % 2) == 1) && (day > 31)) ||
        ((month < 8) && ((month % 2) == 0) && (day > 30)) ||
        ((month > 7) && ((month % 2) == 0) && (day > 31)) ||
        ((month > 7) && ((month % 2) == 1) && (day > 30)) ||
        ((month == 2) && ((year % 4) != 0) && (day > 28)) ||
        ((month == 2) && ((year % 4) == 0) && (day > 29))) {

        return false;
    }
    return true;
}

time_t parse_rfc1123_date_time(int & character, httpscan_t * scanner)
{
    character = get_next_non_whitespace(scanner);
    const int day = parse_unsigned_integer(character, scanner);

    if ((character == ' ') || (character == '\n')) {
        character = get_next_non_whitespace(scanner);
    }
    const char c1 = static_cast<char>(character);
    const char c2 = static_cast<char>(get_normalized_char(scanner));
    const char c3 = static_cast<char>(get_normalized_char(scanner));
    const int month = parse_month(c1, c2, c3);

    character = get_next_non_whitespace(scanner);
    const int year = parse_unsigned_integer(character, scanner);

    if ((character == ' ') || (character == '\n')) {
        character = get_next_non_whitespace(scanner);
    }
    const int second_of_day = parse_time(character, scanner);

    if ((character == ' ') || (character == '\n')) {
        character = get_next_non_whitespace(scanner);
    }
    if ((false == compare_case_insensitive('g', static_cast<char>(character))) ||
        (false == verify_forced_characters(lower_case_string_mt(), scanner))) {
        return -1;
    }
    character = get_next_non_whitespace(scanner);

    if (false == is_valid_epoch_date(day, month, year)) {
        return -1;
    }
    return seconds_since_epoch(second_of_day, day, month, year);
}

time_t parse_rfc850_date_time(int & character, httpscan_t * scanner)
{
    character = get_next_non_whitespace(scanner);
    const int day = parse_unsigned_integer(character, scanner);

    if (character != '-') {
        return -1;
    }
    character = get_normalized_char(scanner);
    const char c1 = static_cast<char>(character);
    const char c2 = static_cast<char>(get_normalized_char(scanner));
    const char c3 = static_cast<char>(get_normalized_char(scanner));
    const int month = parse_month(c1, c2, c3);

    character = get_normalized_char(scanner);
    if (character != '-') {
        return -1;
    }
    character = get_normalized_char(scanner);
    const int year = 1900 + parse_unsigned_integer(character, scanner);
    if ((year < 1900) || (year > 1999)) {
        return -1;
    }

    if ((character == ' ') || (character == '\n')) {
        character = get_next_non_whitespace(scanner);
    }
    const int second_of_day = parse_time(character, scanner);

    if ((character == ' ') || (character == '\n')) {
        character = get_next_non_whitespace(scanner);
    }
    if ((false == compare_case_insensitive('g', static_cast<char>(character))) ||
        (false == verify_forced_characters(lower_case_string_mt(), scanner))) {
        return -1;
    }
    character = get_next_non_whitespace(scanner);

    if (false == is_valid_epoch_date(day, month, year)) {
        return -1;
    }
    return seconds_since_epoch(second_of_day, day, month, year);
}

time_t parse_asctime_date_time(int & character, httpscan_t * scanner)
{
    if ((character == ' ') || (character == '\n')) {
        character = get_next_non_whitespace(scanner);
    }
    const char c1 = static_cast<char>(character);
    const char c2 = static_cast<char>(get_normalized_char(scanner));
    const char c3 = static_cast<char>(get_normalized_char(scanner));
    const int month = parse_month(c1, c2, c3);

    character = get_next_non_whitespace(scanner);
    const int day = parse_unsigned_integer(character, scanner);

    character = get_next_non_whitespace(scanner);
    const int second_of_day = parse_time(character, scanner);

    character = get_next_non_whitespace(scanner);
    const int year = parse_unsigned_integer(character, scanner);

    if (character == ' ') {
        character = get_next_non_whitespace(scanner);
    }

    if (false == is_valid_epoch_date(day, month, year)) {
        return -1;
    }
    return seconds_since_epoch(second_of_day, day, month, year);
}

int parse_weekday(int & character, httpscan_t * scanner)
{
    const char c1 = static_cast<char>(character);
    const char c2 = static_cast<char>(get_normalized_char(scanner));
    const char c3 = static_cast<char>(get_normalized_char(scanner));

    if ((true == compare_case_insensitive('s', c1))) {
        if ((true == compare_case_insensitive('a', c2)) &&
            (true == compare_case_insensitive('t', c3))) {
            return 6;
        } else if ((true == compare_case_insensitive('u', c2)) &&
                   (true == compare_case_insensitive('n', c3))) {
            return 0;
        }
    } else if ((true == compare_case_insensitive('t', c1))) {
        if ((true == compare_case_insensitive('u', c2)) &&
            (true == compare_case_insensitive('e', c3))) {
            return 2;
        } else if ((true == compare_case_insensitive('h', c2)) &&
                   (true == compare_case_insensitive('u', c3))) {
            return 4;
        }
    } else if ((true == compare_case_insensitive('f', c1)) &&
               (true == compare_case_insensitive('r', c2)) &&
               (true == compare_case_insensitive('i', c3))) {
        return 5;
    } else if ((true == compare_case_insensitive('m', c1)) &&
               (true == compare_case_insensitive('o', c2)) &&
               (true == compare_case_insensitive('n', c3))) {
        return 1;
    } else if ((true == compare_case_insensitive('w', c1)) &&
               (true == compare_case_insensitive('e', c2)) &&
               (true == compare_case_insensitive('d', c3))) {
        return 3;
    }
    return -1;
}

LOWER_CASE_STRING(day);
LOWER_CASE_STRING(sday);
LOWER_CASE_STRING(nesday);
LOWER_CASE_STRING(rsday);
LOWER_CASE_STRING(urday);

time_t parse_timestamp(int & character, httpscan_t * scanner)
{
    const int weekday = parse_weekday(character, scanner);
    character = scanner->peek_functor_();
    if (character == ' ') {
        character = get_normalized_char(scanner);
        return parse_asctime_date_time(character, scanner);
    } else if (character == ',') {
        character = get_normalized_char(scanner);
        return parse_rfc1123_date_time(character, scanner);
    } else {
        switch (weekday) {
        case 0:
        case 1:
        case 5:
            if (false == verify_forced_characters(lower_case_string_day(), scanner)) {
                return -1;
            }
            break;

        case 2:
            if (false == verify_forced_characters(lower_case_string_sday(), scanner)) {
                return -1;
            }
            break;

        case 3:
            if (false == verify_forced_characters(lower_case_string_nesday(), scanner)) {
                return -1;
            }
            break;

        case 4:
            if (false == verify_forced_characters(lower_case_string_rsday(), scanner)) {
                return -1;
            }
            break;

        case 6:
            if (false == verify_forced_characters(lower_case_string_urday(), scanner)) {
                return -1;
            }
            break;

        default:
            return -1;
        }

        character = get_next_non_whitespace(scanner);
        if (character != ',') {
            return -1;
        }

        character = get_next_non_whitespace(scanner);
        return parse_rfc850_date_time(character, scanner);
    }
    return -1;
}

bool parse_header_key_compared_to_string(int & character,
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

    return (i == j) && (i == string.size());
}

bool parse_header_type(int & result, httpscan_t * scanner)
{
    do {
        if ((result < 0) || (false == is_valid_header_key_character(static_cast<uint8_t>(result)))) {
            result = -1;
            return false;
        }
        scanner->header_key_.push_back(to_lower(static_cast<char>(result)));
        result = get_normalized_char(scanner);
    } while (result != ':');

    return true;
}

bool parse_header_value(int & result, httpscan_t * scanner)
{
    while (result != '\n') {
        if ((result < 0) ||
            (false == is_valid_header_value_character(static_cast<uint8_t>(result)))) {
            return false;
        }
        scanner->header_value_.push_back(static_cast<char>(result));
        result = get_normalized_char(scanner);
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

bool parse_header(int & result, httpscan_t * scanner)
{
    /*if ((result == 'a') || (result == 'A')) {
        ;
    } else */if ((result == 'c') || (result == 'C')) {
        bool equal = parse_header_key_compared_to_string(result, "c", "ontent-length", scanner);
        if ((true == equal) && (result == ':')) {
            result = get_next_non_whitespace(scanner);
            int code = parse_unsigned_integer(result, scanner);
            if (code < 0) {
                return false;
            }

            scanner->content_length_ = static_cast<size_t>(code);
            return true;
        }
    } else if ((result == 'd') || (result == 'D')) {
        bool equal = parse_header_key_compared_to_string(result, "d", "ate", scanner);
        if ((true == equal) && (result == ':')) {
            result = get_next_non_whitespace(scanner);
            time_t date = parse_timestamp(result, scanner);
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
    result = get_normalized_char(scanner);
    if (false == parse_header_value(result, scanner)) {
        return false;
    }
    return true;
}

LOWER_CASE_STRING(ead);
LOWER_CASE_STRING(et);
LOWER_CASE_STRING(t);
LOWER_CASE_STRING(st);
LOWER_CASE_STRING(elete);
LOWER_CASE_STRING(ptions);
LOWER_CASE_STRING(onnect);
LOWER_CASE_STRING(race);

bool lex_request_method(int & result, httpscan_t * scanner)
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

    return succeeded;
}

bool lex_request_url(int & result, httpscan_t * scanner)
{
    int character = result;
    do {
        if ((character < 0) || (false == is_valid_url_character(static_cast<uint8_t>(character)))) {
            return false;
        }
        scanner->url_.push_back(static_cast<char>(character));
        character = get_normalized_char(scanner);
    } while ((character != ' ') && (character != '\t'));
    return true;
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

bool lex_status_code(int & result, httpscan_t * scanner)
{
    int code = parse_unsigned_integer(result, scanner);
    if (code < 0) {
        return false;
    }
    scanner->status_code_ = static_cast<uint16_t>(code);
    return true;
}

bool lex_reason_phrase(int & result, httpscan_t * scanner)
{
    int character = result;
    do {
        if ((character < 0) ||
            ((character != ' ') && (character != '\t') && (0 == isalnum(character)))) {
            return false;
        }
        scanner->reason_phrase_.push_back(static_cast<char>(character));
        character = get_normalized_char(scanner);
    } while (character != '\n');
    return true;
}

bool parse_headers(int & result, httpscan_t * scanner)
{
    while (result != '\n') {
        if (false == parse_header(result, scanner)) {
            return false;
        }
        result = get_normalized_char(scanner);
    }

    return true;
}

lexer_state lex_first_line(httpscan_t * scanner)
{
    int result = get_next_non_whitespace(scanner);
    if (result < 0) {
        return lexer_state::ERROR;
    }
    int peek_result = scanner->peek_functor_();

    if (((result == 'h') || (result == 'H')) &&
        ((peek_result == 't') || (peek_result == 'T'))) {
        if (false == lex_http_version(result, scanner)) {
            return lexer_state::ERROR;
        }
        result = get_next_non_whitespace(scanner);
        if (false == lex_status_code(result, scanner)) {
            return lexer_state::ERROR;
        }
        result = get_next_non_whitespace(scanner);
        if (false == lex_reason_phrase(result, scanner)) {
            return lexer_state::ERROR;
        }
    } else {
        if (false == lex_request_method(result, scanner)) {
            return lexer_state::ERROR;
        }
        result = get_next_non_whitespace(scanner);
        if (false == lex_request_url(result, scanner)) {
            return lexer_state::ERROR;
        }
        result = get_next_non_whitespace(scanner);
        if (false == lex_http_version(result, scanner)) {
            return lexer_state::ERROR;
        }
        const int newline = get_next_non_whitespace(scanner);
        if (newline != '\n') {
            return lexer_state::ERROR;
        }
    }
    result = get_normalized_char(scanner);
    if (false == parse_headers(result, scanner)) {
        return lexer_state::ERROR;
    }
    return lexer_state::FINISHED;
}

void http_parse(httpscan_t * scanner)
{
    if ((scanner->state_ != lexer_state::FINISHED) &&
        (scanner->state_ != lexer_state::ERROR)) {
        scanner->state_ = lex_first_line(scanner);
    }
}
