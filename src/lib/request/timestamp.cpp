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

#include "timestamp.hpp"

#include <cstring>

#include "utility/common.hpp"
#include "utility/date_calculation.hpp"
#include "utility/parsing.hpp"
#include "utility/select_char_map.hpp"
#include "utility/trie.hpp"

namespace hutzn
{

namespace
{

static const int32_t hours_per_day = 24;
static const int32_t minutes_per_hour = 60;
static const int32_t seconds_per_minute = 60;
static const int32_t begin_of_19_hundrets = 1900;
static const int32_t end_of_19_hundrets = 1999;
static const size_t trie_string_max_length = 16;

using weekday_value_type = std::tuple<int8_t, bool>;

trie<weekday_value_type> make_weekday_trie(size_t& max_size)
{
    trie<weekday_value_type> result{true};

    // Filling versions and automatically calculate the maximum length.
    max_size = 0;
    static const std::vector<std::pair<const char_t* const, weekday_value_type>>
        weekdays = {std::make_pair("sun", weekday_value_type{0, false}),
                    std::make_pair("sunday", weekday_value_type{0, true}),
                    std::make_pair("mon", weekday_value_type{1, false}),
                    std::make_pair("monday", weekday_value_type{1, true}),
                    std::make_pair("tue", weekday_value_type{2, false}),
                    std::make_pair("tuesday", weekday_value_type{2, true}),
                    std::make_pair("wed", weekday_value_type{3, false}),
                    std::make_pair("wednesday", weekday_value_type{3, true}),
                    std::make_pair("thu", weekday_value_type{4, false}),
                    std::make_pair("thursday", weekday_value_type{4, true}),
                    std::make_pair("fri", weekday_value_type{5, false}),
                    std::make_pair("friday", weekday_value_type{5, true}),
                    std::make_pair("sat", weekday_value_type{6, false}),
                    std::make_pair("saturday", weekday_value_type{6, true})};
    for (const std::pair<const char_t* const, weekday_value_type>& pair :
         weekdays) {
        result.insert(pair.first, pair.second);
        max_size =
            std::max(max_size, ::strnlen(pair.first, trie_string_max_length));
    }

    return result;
}

trie<int32_t> make_month_trie(size_t& max_size)
{
    trie<int32_t> result{true};

    // Filling versions and automatically calculate the maximum length.
    max_size = 0;
    static const std::vector<std::pair<const char_t* const, int32_t>> months = {
        std::make_pair("jan", 1),  std::make_pair("feb", 2),
        std::make_pair("mar", 3),  std::make_pair("apr", 4),
        std::make_pair("may", 5),  std::make_pair("jun", 6),
        std::make_pair("jul", 7),  std::make_pair("aug", 8),
        std::make_pair("sep", 9),  std::make_pair("oct", 10),
        std::make_pair("nov", 11), std::make_pair("dec", 12)};
    for (const std::pair<const char_t* const, int32_t>& pair : months) {
        result.insert(pair.first, pair.second);
        max_size =
            std::max(max_size, ::strnlen(pair.first, trie_string_max_length));
    }

    return result;
}

int32_t parse_time(const char_t*& data, size_t& remaining)
{
    const int32_t hour = parse_unsigned_integer<int32_t>(data, remaining);
    skip_whitespace(data, remaining);
    if ((!check_range<int32_t, 0, hours_per_day - 1>(hour)) ||
        ((*data) != ':')) {
        return -1;
    }
    skip_one_character(data, remaining);
    skip_whitespace(data, remaining);
    const int32_t minute = parse_unsigned_integer<int32_t>(data, remaining);
    skip_whitespace(data, remaining);
    if ((!check_range<int32_t, 0, minutes_per_hour - 1>(minute)) ||
        ((*data) != ':')) {
        return -1;
    }
    skip_one_character(data, remaining);
    skip_whitespace(data, remaining);
    const int32_t second = parse_unsigned_integer<int32_t>(data, remaining);
    if (!check_range<int32_t, 0, seconds_per_minute - 1>(second)) {
        return -1;
    }
    return (seconds_per_minute * ((minutes_per_hour * hour) + minute)) + second;
}

int32_t parse_month(const char_t*& data, size_t& remaining)
{
    static size_t maximum_month_length = 0;
    static const trie<int32_t> months = make_month_trie(maximum_month_length);

    trie_find_result<int32_t> result = months.find(data, remaining);
    data += result.used_size();
    remaining -= result.used_size();
    return result.value();
}

bool parse_gmt(const char_t*& data, size_t& remaining)
{
    static const size_t max_gmt_length_ = 3;

    bool result;
    if ((remaining >= max_gmt_length_) &&
        (0 == ::strncasecmp(data, "gmt", max_gmt_length_))) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

time_t parse_rfc1123_date_time(const char_t*& data, size_t& remaining)
{
    skip_one_character(data, remaining);
    skip_whitespace(data, remaining);
    const int32_t day = parse_unsigned_integer<int32_t>(data, remaining);

    skip_whitespace(data, remaining);
    const int32_t month = parse_month(data, remaining);

    skip_whitespace(data, remaining);
    const int32_t year = parse_unsigned_integer<int32_t>(data, remaining);

    skip_whitespace(data, remaining);
    const int32_t second_of_day = parse_time(data, remaining);
    if (second_of_day < 0) {
        return -1;
    }

    skip_whitespace(data, remaining);
    if (!parse_gmt(data, remaining)) {
        return -1;
    }

    return seconds_since_epoch(
        static_cast<uint32_t>(second_of_day), static_cast<uint8_t>(day),
        static_cast<uint8_t>(month), static_cast<uint16_t>(year));
}

time_t parse_rfc850_date_time(const char_t*& data, size_t& remaining)
{
    skip_whitespace(data, remaining);
    if ((*data) != ',') {
        return -1;
    }
    skip_one_character(data, remaining);
    skip_whitespace(data, remaining);
    const int32_t day = parse_unsigned_integer<int32_t>(data, remaining);

    if ((*data) != '-') {
        return -1;
    }

    data++;
    remaining--;

    const int32_t month = parse_month(data, remaining);

    if ((*data) != '-') {
        return -1;
    }

    data++;
    remaining--;

    const int32_t year =
        begin_of_19_hundrets + parse_unsigned_integer<int32_t>(data, remaining);
    if (!check_range<int32_t, begin_of_19_hundrets, end_of_19_hundrets>(year)) {
        return -1;
    }

    skip_whitespace(data, remaining);
    const int32_t second_of_day = parse_time(data, remaining);

    skip_whitespace(data, remaining);
    if (!parse_gmt(data, remaining)) {
        return -1;
    }

    return seconds_since_epoch(
        static_cast<uint32_t>(second_of_day), static_cast<uint8_t>(day),
        static_cast<uint8_t>(month), static_cast<uint16_t>(year));
}

time_t parse_asctime_date_time(const char_t*& data, size_t& remaining)
{
    skip_whitespace(data, remaining);
    const int32_t month = parse_month(data, remaining);

    skip_one_character(data, remaining);
    skip_whitespace(data, remaining);
    const int32_t day = parse_unsigned_integer<int32_t>(data, remaining);

    skip_one_character(data, remaining);
    skip_whitespace(data, remaining);
    const int32_t second_of_day = parse_time(data, remaining);

    skip_one_character(data, remaining);
    skip_whitespace(data, remaining);
    const int32_t year = parse_unsigned_integer<int32_t>(data, remaining);

    return seconds_since_epoch(
        static_cast<uint32_t>(second_of_day), static_cast<uint8_t>(day),
        static_cast<uint8_t>(month), static_cast<uint16_t>(year));
}

weekday_value_type parse_weekday(const char_t*& data, size_t& remaining)
{
    static size_t maximum_weekday_length = 0;
    static const trie<weekday_value_type> weekdays =
        make_weekday_trie(maximum_weekday_length);

    skip_whitespace(data, remaining);
    trie_find_result<weekday_value_type> result =
        weekdays.find(data, remaining);
    data += result.used_size();
    remaining -= result.used_size();
    return result.value();
}

} // namespace

time_t parse_timestamp(const char_t* data, size_t length)
{
    int32_t weekday = -1;
    int32_t is_long_format = false;
    std::tie(weekday, is_long_format) = parse_weekday(data, length);
    if (is_long_format) {
        return parse_rfc850_date_time(data, length);
    } else if (((*data) == ' ') || ((*data) == '\t')) {
        return parse_asctime_date_time(data, length);
    } else {
        return parse_rfc1123_date_time(data, length);
    }
}

} // namespace hutzn
