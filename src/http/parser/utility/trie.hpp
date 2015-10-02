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

#ifndef LIBHUTZNOHMD_HTTP_PARSER_UTILITY_TRIE_HPP
#define LIBHUTZNOHMD_HTTP_PARSER_UTILITY_TRIE_HPP

#include <array>
#include <memory>
#include <tuple>
#include <vector>

#include <utility/character_validation.hpp>

#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/push_back_string.hpp>
#include <utility/common.hpp>

namespace hutzn
{

namespace http
{

//! Provides a parsing algorithm to parse a string into a enum value.
template <typename value_type>
class trie
{
public:
    using value_info = std::tuple<const char_t* const, value_type>;

    explicit trie(const std::vector<value_info>& values,
                  const value_type& default_value);

    template <size_t size>
    value_type parse(int32_t& character,
                     push_back_string<size>& fail_safe_result,
                     const lexer& l) const;

private:
    explicit trie(const std::vector<value_info>& values,
                  const std::string& name, const size_t& index,
                  const value_type& default_value);

    const bool has_value_;
    const std::string name_;
    const value_type value_;
    std::array<std::unique_ptr<trie>, 256> children_;
};

template <typename value_type>
trie<value_type>::trie(const std::vector<value_info>& values,
                       const value_type& default_value)
    : trie(values, "", 0, default_value)
{
}

template <typename value_type>
template <size_t size>
value_type trie<value_type>::parse(int32_t& character,
                                   push_back_string<size>& fail_safe_result,
                                   const lexer& l) const
{
    const std::unique_ptr<trie>& child =
        children_[static_cast<uint8_t>(character)];
    if (std::unique_ptr<trie>() == child) {
        if (!has_value_) {
            fail_safe_result.append_string(name_.c_str());
        }
        return value_;
    }

    character = l.get();
    return child->parse(character, fail_safe_result, l);
}

template <typename value_info>
void fill_next_values(std::vector<value_info>& next_values,
                      const std::vector<value_info>& values,
                      const size_t& index, const uint8_t& c)
{
    for (size_t j = 0; j < values.size(); j++) {
        const value_info& w = values[j];
        const char_t* const next_string = std::get<0>(w);
        if ((c != 0) && (static_cast<uint8_t>(next_string[index]) == c)) {
            next_values.push_back(w);
        }
    }
}

template <typename value_type>
trie<value_type>::trie(const std::vector<value_info>& values,
                       const std::string& name, const size_t& index,
                       const value_type& default_value)
    : has_value_(false)
    , name_(name)
    , value_(default_value)
    , children_()
{
    for (size_t i = 0; i < values.size(); i++) {
        const value_info& v = values[i];
        const char_t* const string = std::get<0>(v);
        const value_type& value = std::get<1>(v);
        const uint8_t c = static_cast<uint8_t>(string[index]);

        if ('\0' == c) {
            const_cast<bool&>(has_value_) = true;
            const_cast<value_type&>(value_) = value;
            continue;
        }

        if (std::unique_ptr<trie>() != children_[c]) {
            continue;
        }

        // Because we already carried all children going on with that character,
        // we must not do
        // this again.
        std::vector<value_info> next_values;
        fill_next_values(next_values, values, index, c);

        if (!next_values.empty()) {
            children_[c] = std::unique_ptr<trie>(
                new trie(next_values, name_ + static_cast<char_t>(c), index + 1,
                         default_value));

            // The trie parsing is case insensitive.
            if (check_range<uint8_t, 'a', 'z'>(c)) {
                children_[c & 0xDFU] = std::unique_ptr<trie>(new trie(
                    next_values, name_ + static_cast<char_t>(c & 0xDFU),
                    index + 1, default_value));
            } else if (check_range<uint8_t, 'A', 'Z'>(c)) {
                children_[c | 0x20U] = std::unique_ptr<trie>(new trie(
                    next_values, name_ + static_cast<char_t>(c | 0x20U),
                    index + 1, default_value));
            }
        }
    }
}

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HTTP_PARSER_UTILITY_TRIE_HPP
