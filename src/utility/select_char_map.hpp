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

#ifndef LIBHUTZNOHMD_UTILITY_SELECT_CHAR_MAP_HPP
#define LIBHUTZNOHMD_UTILITY_SELECT_CHAR_MAP_HPP

#include <array>
#include <bitset>
#include <limits>
#include <type_traits>

#include <libhutznohmd/types.hpp>

namespace hutzn
{

namespace detail
{

using select_char_map_base_type = char_t;

using select_char_map_unsigned_type = std::make_unsigned<char_t>::type;

} // namespace detail

using select_char_map = std::bitset<
    std::numeric_limits<detail::select_char_map_unsigned_type>::max() + 1>;

namespace detail
{

//! @brief Is used as termination function, that is doing nothing.
void fill_select_char_map(select_char_map&)
{
}

//! @brief Sets a selection character in a select_char_map to true and calls the
//! function for all other given characters too.
template <typename... tn>
void fill_select_char_map(select_char_map& result,
                          const select_char_map_base_type selection_character,
                          const tn... selection_characters)
{
    result[static_cast<uint8_t>(selection_character)] = true;
    fill_select_char_map(result, selection_characters...);
}

} // namespace detail

//! @brief Creates and returns a select_char_map, which maps a character to a
//! boolean value.
template <typename... tn>
select_char_map make_select_char_map(const tn... stop_chars)
{
    select_char_map result;
    detail::fill_select_char_map(result, stop_chars...);
    return result;
}

} // namespace hutzn

#endif // LIBHUTZNOHMD_UTILITY_SELECT_CHAR_MAP_HPP
