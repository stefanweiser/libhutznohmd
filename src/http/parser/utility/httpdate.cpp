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

#include <http/parser/utility/trie.hpp>

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

int32_t parse_month(int32_t & character, const lexer & l)
{
    using value_info = trie<int32_t>::value_info;
    static const std::vector<value_info> types = {{
            value_info{"jan", 1},
            value_info{"feb", 2},
            value_info{"mar", 3},
            value_info{"apr", 4},
            value_info{"may", 5},
            value_info{"jun", 6},
            value_info{"jul", 7},
            value_info{"aug", 8},
            value_info{"sep", 9},
            value_info{"oct", 10},
            value_info{"nov", 11},
            value_info{"dec", 12}
        }
    };

    static const trie<int32_t> t(types, -1);
    push_back_string<32> tmp;
    return t.parse(character, tmp, l);
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

bool parse_gmt(int32_t & character, const lexer & l)
{
    using value_info = trie<bool>::value_info;
    static const std::vector<value_info> types = {{
            value_info{"gmt", true}
        }
    };

    static const trie<bool> t(types, false);
    push_back_string<32> tmp;
    return t.parse(character, tmp, l);
}

time_t parse_rfc1123_date_time(int32_t & character, const lexer & l)
{
    character = l.get_non_whitespace();
    const int32_t day = l.get_unsigned_integer(character);

    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
    const int32_t month = parse_month(character, l);

    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
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
    if (false == parse_gmt(character, l)) {
        return -1;
    }

    if (false == is_valid_epoch_date(day, month, year)) {
        return -1;
    }
    return seconds_since_epoch(second_of_day, day, month, year);
}

time_t parse_rfc850_date_time(int32_t & character, const lexer & l)
{
    if ((character == ' ') || (character == '\n')) {
        character = l.get_non_whitespace();
    }
    if (character != ',') {
        return -1;
    }
    character = l.get_non_whitespace();
    const int32_t day = l.get_unsigned_integer(character);

    if (character != '-') {
        return -1;
    }
    character = l.get();
    const int32_t month = parse_month(character, l);

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
    if (false == parse_gmt(character, l)) {
        return -1;
    }

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
    const int32_t month = parse_month(character, l);

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

std::tuple<int8_t, bool> parse_weekday(int32_t & character, const lexer & l)
{
    using value_type = std::tuple<int8_t, bool>;
    using value_info = trie<value_type>::value_info;

    static const std::vector<value_info> types = {{
            value_info{"sun", value_type{0, false}},
            value_info{"sunday", value_type{0, true}},
            value_info{"mon", value_type{1, false}},
            value_info{"monday", value_type{1, true}},
            value_info{"tue", value_type{2, false}},
            value_info{"tuesday", value_type{2, true}},
            value_info{"wed", value_type{3, false}},
            value_info{"wednesday", value_type{3, true}},
            value_info{"thu", value_type{4, false}},
            value_info{"thursday", value_type{4, true}},
            value_info{"fri", value_type{5, false}},
            value_info{"friday", value_type{5, true}},
            value_info{"sat", value_type{6, false}},
            value_info{"saturday", value_type{6, true}}
        }
    };

    static const trie<value_type> t(types, value_type { -1, false});
    push_back_string<32> tmp;
    return t.parse(character, tmp, l);
}

time_t parse_timestamp(int32_t & character, const lexer & l)
{
    int32_t weekday = -1;
    int32_t is_long_format = false;
    std::tie(weekday, is_long_format) = parse_weekday(character, l);
    if (true == is_long_format) {
        return parse_rfc850_date_time(character, l);
    } else if ((character == ' ') || (character == '\t')) {
        return parse_asctime_date_time(character, l);
    } else {
        return parse_rfc1123_date_time(character, l);
    }
    return -1;
}

} // namespace http

} // namespace rest
