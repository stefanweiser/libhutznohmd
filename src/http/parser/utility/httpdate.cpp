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

#include "httpdate.hpp"

namespace rest
{

namespace http
{

int32_t parse_time(int32_t & character, const lexer & l)
{
    int32_t hour = l.get_unsigned_integer(character);
    if ((hour < 0) || (hour > 23) || (character != ':')) {
        return -1;
    }
    character = l.get_non_whitespace();
    int32_t minute = l.get_unsigned_integer(character);
    if ((minute < 0) || (minute > 59) || (character != ':')) {
        return -1;
    }
    character = l.get_non_whitespace();
    int32_t second = l.get_unsigned_integer(character);
    if ((second < 0) || (second > 59)) {
        return -1;
    }
    return (60 * ((60 * hour) + minute)) + second;
}

int32_t parse_month(const char c1, const char c2, const char c3)
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

LOWER_CASE_STRING(mt);

time_t parse_rfc1123_date_time(int32_t & character, const lexer & l)
{
    character = l.get_non_whitespace();
    const int32_t day = l.get_unsigned_integer(character);

    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
    const char c1 = static_cast<char>(character);
    const char c2 = static_cast<char>(l.get());
    const char c3 = static_cast<char>(l.get());
    const int32_t month = parse_month(c1, c2, c3);

    character = l.get_non_whitespace();
    const int32_t year = l.get_unsigned_integer(character);

    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
    const int32_t second_of_day = parse_time(character, l);
    if (second_of_day < 0) {
        return -1;
    }

    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
    if ((false == compare_case_insensitive('g', character)) ||
        (false == verify_forced_characters(lower_case_string_mt(), l))) {
        return -1;
    }
    character = l.get_non_whitespace();

    if (false == is_valid_epoch_date(day, month, year)) {
        return -1;
    }
    return seconds_since_epoch(second_of_day, day, month, year);
}

time_t parse_rfc850_date_time(int32_t & character, const lexer & l)
{
    character = l.get_non_whitespace();
    const int32_t day = l.get_unsigned_integer(character);

    if (character != '-') {
        return -1;
    }
    character = l.get();
    const char c1 = static_cast<char>(character);
    const char c2 = static_cast<char>(l.get());
    const char c3 = static_cast<char>(l.get());
    const int32_t month = parse_month(c1, c2, c3);

    character = l.get();
    if (character != '-') {
        return -1;
    }
    character = l.get();
    const int32_t year = 1900 + l.get_unsigned_integer(character);
    if ((year < 1900) || (year > 1999)) {
        return -1;
    }

    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
    const int32_t second_of_day = parse_time(character, l);

    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
    if ((false == compare_case_insensitive('g', character)) ||
        (false == verify_forced_characters(lower_case_string_mt(), l))) {
        return -1;
    }
    character = l.get_non_whitespace();

    if (false == is_valid_epoch_date(day, month, year)) {
        return -1;
    }
    return seconds_since_epoch(second_of_day, day, month, year);
}

time_t parse_asctime_date_time(int32_t & character, const lexer & l)
{
    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
    const char c1 = static_cast<char>(character);
    const char c2 = static_cast<char>(l.get());
    const char c3 = static_cast<char>(l.get());
    const int32_t month = parse_month(c1, c2, c3);

    character = l.get_non_whitespace();
    const int32_t day = l.get_unsigned_integer(character);

    character = l.get_non_whitespace();
    const int32_t second_of_day = parse_time(character, l);

    character = l.get_non_whitespace();
    const int32_t year = l.get_unsigned_integer(character);

    if (character == ' ') {
        character = l.get_non_whitespace();
    }

    if (false == is_valid_epoch_date(day, month, year)) {
        return -1;
    }
    return seconds_since_epoch(second_of_day, day, month, year);
}

int32_t parse_weekday(int32_t & character, const lexer & l)
{
    const char c1 = static_cast<char>(character);
    const char c2 = static_cast<char>(l.get());
    const char c3 = static_cast<char>(l.get());

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

LOWER_CASE_STRING(ay);
LOWER_CASE_STRING(day);
LOWER_CASE_STRING(esday);
LOWER_CASE_STRING(sday);
LOWER_CASE_STRING(rday);

time_t parse_timestamp(int32_t & character, const lexer & l)
{
    const int32_t weekday = parse_weekday(character, l);
    character = l.get();
    if (character == ' ') {
        return parse_asctime_date_time(character, l);
    } else if (character == ',') {
        return parse_rfc1123_date_time(character, l);
    } else {
        switch (weekday) {
        case 0:
        case 1:
        case 5:
            if ((false == compare_case_insensitive('d', character)) ||
                (false == verify_forced_characters(lower_case_string_ay(), l))) {
                return -1;
            }
            break;

        case 2:
            if ((false == compare_case_insensitive('s', character)) ||
                (false == verify_forced_characters(lower_case_string_day(), l))) {
                return -1;
            }
            break;

        case 3:
            if ((false == compare_case_insensitive('n', character)) ||
                (false == verify_forced_characters(lower_case_string_esday(), l))) {
                return -1;
            }
            break;

        case 4:
            if ((false == compare_case_insensitive('r', character)) ||
                (false == verify_forced_characters(lower_case_string_sday(), l))) {
                return -1;
            }
            break;

        case 6:
            if ((false == compare_case_insensitive('u', character)) ||
                (false == verify_forced_characters(lower_case_string_rday(), l))) {
                return -1;
            }
            break;

        default:
            return -1;
        }

        character = l.get_non_whitespace();
        if (character != ',') {
            return -1;
        }

        return parse_rfc850_date_time(character, l);
    }
    return -1;
}

} // namespace http

} // namespace rest
