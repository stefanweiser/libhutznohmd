/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#ifndef LIBREST_HTTP_PARSER_UTILITY_BASE_PARSER_HPP
#define LIBREST_HTTP_PARSER_UTILITY_BASE_PARSER_HPP

#include <http/parser/utility/anonymous_int_function.hpp>
#include <http/parser/utility/lexer.hpp>
#include <http/parser/utility/media_type.hpp>
#include <http/parser/utility/push_back_string.hpp>
#include <http/parser/utility/trie.hpp>

#include <rest.hpp>

namespace rest
{

namespace http
{

enum class parser_state { UNFINISHED = 0, SUCCEEDED = 1, ERROR = 2 };

enum class connection_type { ERROR = 0, CLOSE, KEEP_ALIVE };

class base_parser
{
public:
    explicit base_parser(const anonymous_int_function& get_functor,
                         const anonymous_int_function& peek_functor);
    virtual ~base_parser();

    bool parse_connection(int32_t& character);
    bool parse_content_length(int32_t& character);
    bool parse_content_md5(int32_t& character);
    bool parse_content_type(int32_t& character);
    bool parse_date(int32_t& character);
    bool parse_custom_header(int32_t& character);

    template <typename parser_class>
    using parsing_function = bool (parser_class::*)(int32_t&);

    template <typename parser_class>
    using trie_value = std::tuple<parsing_function<parser_class>, size_t>;

    template <typename parser_class>
    using trie_value_info = typename trie<trie_value<parser_class>>::value_info;

    template <class parser>
    bool parse_generic_header(const std::vector<trie_value_info<parser>>& types,
                              parser& p, int32_t& character);

    lexer lexer_;
    parser_state state_;

    push_back_string<40> header_key_;
    push_back_string<1000> header_value_;
    rest::http::version version_;
    std::map<std::string, std::string> headers_;
    size_t content_length_;
    media_type content_type_;
    time_t date_;
    connection_type connection_;
    std::array<uint8_t, 16> md5_;
    bool has_md5_;
};

template <class parser>
bool base_parser::parse_generic_header(
    const std::vector<base_parser::trie_value_info<parser>>& types, parser& p,
    int32_t& character)
{
    using trie_value_ = trie_value<parser>;
    static const trie<trie_value_> t(types, trie_value_{nullptr, -1});
    trie_value_ value = t.parse(character, header_key_, lexer_);
    if (character == ':') {
        character = lexer_.get_non_whitespace();
        parsing_function<parser> functor = std::get<0>(value);
        if (nullptr != functor) {
            return (p.*functor)(character);
        }
    }

    size_t index = std::get<1>(value);
    if (index < types.size()) {
        header_key_.append_string(std::get<0>(types[index]));
    }

    return parse_custom_header(character);
}

} // namespace http

} // namespace rest

#endif // LIBREST_HTTP_PARSER_UTILITY_BASE_PARSER_HPP
