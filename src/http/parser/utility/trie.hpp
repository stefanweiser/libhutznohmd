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

#ifndef __LIBREST_HTTP_PARSER_UTILITY_TOKENPARSER_HPP__
#define __LIBREST_HTTP_PARSER_UTILITY_TOKENPARSER_HPP__

#include <array>
#include <memory>
#include <vector>

#include <http/parser/utility/charactercompare.hpp>
#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/pushbackstring.hpp>

namespace rest
{

namespace http
{

class trie
{
public:
    explicit trie(const std::vector<const char *> & strings,
                  const std::string & name,
                  const size_t & index);

    template<typename continue_function, size_t size>
    bool parse(int32_t & character,
               push_back_string<size> & fail_safe_result,
               const continue_function & continue_condition_functor,
               const lexer & l);

private:
    bool is_leaf_;
    const std::string name_;
    std::array<std::unique_ptr<trie>, 256> children_;
};

inline trie::trie(const std::vector<const char *> & strings,
                  const std::string & name,
                  const size_t & index)
    : is_leaf_(true)
    , name_(name)
    , children_()
{
    for (size_t i = 0; i < strings.size(); i++) {
        const char * string = strings[i];
        const uint8_t c = static_cast<uint8_t>(string[index]);
        if ((c != 0) && (c < children_.size()) && (std::unique_ptr<trie>() == children_[c])) {
            std::vector<const char *> next_strings;
            for (size_t j = 0; j < strings.size(); j++) {
                const char * next_string = strings[j];
                if (next_string[index] == c) {
                    next_strings.push_back(next_string);
                }
            }
            if (false == next_strings.empty()) {
                is_leaf_ = false;
                children_[c] = std::unique_ptr<trie>(new trie(next_strings, name_ + static_cast<char>(c),
                                                     index + 1));
                if ((c >= 'a') && (c <= 'z')) {
                    children_[c & 0xDF] = std::unique_ptr<trie>(new trie(next_strings, name_ + static_cast<char>(c),
                                          index + 1));
                }
            }
        }
    }
}

template<typename continue_function, size_t size>
bool trie::parse(int32_t & character,
                 push_back_string<size> & fail_safe_result,
                 const continue_function & continue_condition_functor,
                 const lexer & l)
{
    if (false == continue_condition_functor(static_cast<char>(character))) {
        if (true == is_leaf_) {
            return true;
        }

        fail_safe_result.push_back(name_.c_str());
        return false;
    }

    const std::unique_ptr<trie> & child = children_[static_cast<uint8_t>(character)];
    if (std::unique_ptr<trie>() == child) {
        fail_safe_result.push_back(name_.c_str());
        return false;
    }

    character = l.get();
    return child->parse(character, fail_safe_result, continue_condition_functor, l);
}

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_PARSER_UTILITY_TOKENPARSER_HPP__
