/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

 * The libhutznohmd project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The libhutznohmd project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the libhutznohmd project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <http/parser/utility/trie.hpp>
#include <utility/common.hpp>

#include "timestamp.hpp"

namespace hutzn
{

namespace http
{

namespace
{

static const int32_t hours_per_day = 24;
static const int32_t minutes_per_hour = 60;
static const int32_t seconds_per_minute = 60;
static const int32_t begin_of_19_hundrets = 1900;
static const int32_t end_of_19_hundrets = 1999;
static const size_t tmp_string_size = 32;

int32_t parse_time(int32_t& character, const lexer& lex)
{
    int32_t hour = lex.get_unsigned_integer(character);
    if ((!check_range<int32_t, 0, hours_per_day - 1>(hour)) ||
        (character != ':')) {
        return -1;
    }
    character = lex.get_non_whitespace();
    int32_t minute = lex.get_unsigned_integer(character);
    if ((!check_range<int32_t, 0, minutes_per_hour - 1>(minute)) ||
        (character != ':')) {
        return -1;
    }
    character = lex.get_non_whitespace();
    int32_t second = lex.get_unsigned_integer(character);
    if (!check_range<int32_t, 0, seconds_per_minute - 1>(second)) {
        return -1;
    }
    return (seconds_per_minute * ((minutes_per_hour * hour) + minute)) + second;
}

int32_t parse_month(int32_t& character, const lexer& lex)
{
    using value_info = trie<int32_t>::value_info;
    static const std::vector<value_info> types = {
        {value_info{"jan", 1}, value_info{"feb", 2}, value_info{"mar", 3},
         value_info{"apr", 4}, value_info{"may", 5}, value_info{"jun", 6},
         value_info{"jul", 7}, value_info{"aug", 8}, value_info{"sep", 9},
         value_info{"oct", 10}, value_info{"nov", 11}, value_info{"dec", 12}}};

    static const trie<int32_t> t(types, -1);
    push_back_string<tmp_string_size> tmp;
    return t.parse(character, tmp, lex);
}

bool parse_gmt(int32_t& character, const lexer& lex)
{
    using value_info = trie<bool>::value_info;
    static const std::vector<value_info> types = {{value_info{"gmt", true}}};

    static const trie<bool> t(types, false);
    push_back_string<tmp_string_size> tmp;
    return t.parse(character, tmp, lex);
}

void parse_optional_whitespace(int32_t& character, const lexer& lex)
{
    if ((character == ' ') || (character == '\n')) {
        character = lex.get_non_whitespace();
    }
}

time_t parse_rfc1123_date_time(int32_t& character, const lexer& lex)
{
    character = lex.get_non_whitespace();
    const int32_t day = lex.get_unsigned_integer(character);

    parse_optional_whitespace(character, lex);
    const int32_t month = parse_month(character, lex);

    parse_optional_whitespace(character, lex);
    const int32_t year = lex.get_unsigned_integer(character);

    parse_optional_whitespace(character, lex);
    const int32_t second_of_day = parse_time(character, lex);
    if (second_of_day < 0) {
        return -1;
    }

    parse_optional_whitespace(character, lex);
    if (!parse_gmt(character, lex)) {
        return -1;
    }

    return seconds_since_epoch(
        static_cast<uint32_t>(second_of_day), static_cast<uint8_t>(day),
        static_cast<uint8_t>(month), static_cast<uint32_t>(year));
}

time_t parse_rfc850_date_time(int32_t& character, const lexer& lex)
{
    parse_optional_whitespace(character, lex);
    if (character != ',') {
        return -1;
    }
    character = lex.get_non_whitespace();
    const int32_t day = lex.get_unsigned_integer(character);

    if (character != '-') {
        return -1;
    }
    character = lex.get();
    const int32_t month = parse_month(character, lex);

    if (character != '-') {
        return -1;
    }
    character = lex.get();
    const int32_t year =
        begin_of_19_hundrets + lex.get_unsigned_integer(character);
    if (!check_range<int32_t, begin_of_19_hundrets, end_of_19_hundrets>(year)) {
        return -1;
    }

    parse_optional_whitespace(character, lex);
    const int32_t second_of_day = parse_time(character, lex);

    parse_optional_whitespace(character, lex);
    if (!parse_gmt(character, lex)) {
        return -1;
    }

    return seconds_since_epoch(
        static_cast<uint32_t>(second_of_day), static_cast<uint8_t>(day),
        static_cast<uint8_t>(month), static_cast<uint32_t>(year));
}

time_t parse_asctime_date_time(int32_t& character, const lexer& lex)
{
    parse_optional_whitespace(character, lex);
    const int32_t month = parse_month(character, lex);

    character = lex.get_non_whitespace();
    const int32_t day = lex.get_unsigned_integer(character);

    character = lex.get_non_whitespace();
    const int32_t second_of_day = parse_time(character, lex);

    character = lex.get_non_whitespace();
    const int32_t year = lex.get_unsigned_integer(character);

    if (character == ' ') {
        character = lex.get_non_whitespace();
    }

    return seconds_since_epoch(
        static_cast<uint32_t>(second_of_day), static_cast<uint8_t>(day),
        static_cast<uint8_t>(month), static_cast<uint32_t>(year));
}

std::tuple<int8_t, bool> parse_weekday(int32_t& character, const lexer& lex)
{
    using value_type = std::tuple<int8_t, bool>;
    using value_info = trie<value_type>::value_info;

    static const std::vector<value_info> types = {
        {value_info{"sun", value_type{0, false}},
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
         value_info{"saturday", value_type{6, true}}}};

    static const trie<value_type> t(types, value_type{-1, false});
    push_back_string<tmp_string_size> tmp;
    return t.parse(character, tmp, lex);
}

} // namespace

time_t parse_timestamp(int32_t& character, const lexer& lex)
{
    int32_t weekday = -1;
    int32_t is_long_format = false;
    std::tie(weekday, is_long_format) = parse_weekday(character, lex);
    if (is_long_format) {
        return parse_rfc850_date_time(character, lex);
    } else if ((character == ' ') || (character == '\t')) {
        return parse_asctime_date_time(character, lex);
    } else {
        return parse_rfc1123_date_time(character, lex);
    }
}

} // namespace http

} // namespace hutzn
