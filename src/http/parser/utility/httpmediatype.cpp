/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <array>

#include <http/parser/utility/charactercompare.hpp>
#include <http/parser/utility/trie.hpp>

#include "httpmediatype.hpp"

namespace rest
{

namespace http
{

media_type::media_type(const lexer & l)
    : lexer_(l)
    , type_(media_type_type::CUSTOM)
    , subtype_(media_type_subtype::CUSTOM)
    , custom_type_()
    , custom_subtype_()
    , parameters_()
{}

bool media_type::parse(int32_t & character)
{
    parse_type(character);
    if ('/' != character) {
        return false;
    }
    character = lexer_.get_non_whitespace();
    parse_subtype(character);
    while (';' == character) {
        if (false == parse_parameter(character)) {
            return false;
        }
    }
    return true;
}

media_type_type media_type::type() const
{
    return type_;
}

media_type_subtype media_type::subtype() const
{
    return subtype_;
}

const char * media_type::custom_type() const
{
    return custom_type_.c_str();
}

const char * media_type::custom_subtype() const
{
    return custom_subtype_.c_str();
}

const char * media_type::parameter(const char * key) const
{
    auto it = parameters_.find(key);
    if (it != parameters_.end()) {
        return it->second.c_str();
    }
    return "";
}

void media_type::parse_type(int32_t & character)
{
    using value_info = trie<media_type_type>::value_info;

    // The index of the vector has to be the same as the value of the type, because this largely
    // boosts the conversion below.
    static const std::vector<value_info> types = {{
            value_info{"*", media_type_type::WILDCARD},
            value_info{"application", media_type_type::APPLICATION},
            value_info{"audio", media_type_type::AUDIO},
            value_info{"example", media_type_type::EXAMPLE},
            value_info{"image", media_type_type::IMAGE},
            value_info{"message", media_type_type::MESSAGE},
            value_info{"model", media_type_type::MODEL},
            value_info{"multipart", media_type_type::MULTIPART},
            value_info{"text", media_type_type::TEXT},
            value_info{"video", media_type_type::VIDEO}
        }
    };

    static const trie<media_type_type> t(types, media_type_type::CUSTOM);
    type_ = t.parse(character, custom_type_, lexer_);
    if ((media_type_type::CUSTOM != type_) &&
        (true == is_valid_token_character(static_cast<char>(character)))) {
        custom_type_.push_back(std::get<0>(types[static_cast<int32_t>(type_)]));
        type_ = media_type_type::CUSTOM;
    }

    if (media_type_type::CUSTOM == type_) {
        parse_word(character, custom_type_, &do_nothing, &is_valid_token_character, lexer_);
    }
}

void media_type::parse_subtype(int32_t & character)
{
    using value_info = trie<media_type_subtype>::value_info;

    // The index of the vector has to be the same as the value of the subtype, because this
    // largely boosts the conversion below.
    static const std::vector<value_info> types = {{
            value_info{"*", media_type_subtype::WILDCARD},
            value_info{"plain", media_type_subtype::PLAIN}
        }
    };

    static const trie<media_type_subtype> t(types, media_type_subtype::CUSTOM);
    subtype_ = t.parse(character, custom_subtype_, lexer_);
    if ((media_type_subtype::CUSTOM != subtype_) &&
        (true == is_valid_token_character(static_cast<char>(character)))) {
        custom_subtype_.push_back(std::get<0>(types[static_cast<int32_t>(subtype_)]));
        subtype_ = media_type_subtype::CUSTOM;
    }

    if (media_type_subtype::CUSTOM == subtype_) {
        parse_word(character, custom_subtype_, &do_nothing, &is_valid_token_character, lexer_);
    }
}

bool media_type::parse_parameter(int32_t & character)
{
    push_back_string<16> key;
    push_back_string<16> value;

    character = lexer_.get_non_whitespace();

    parse_word(character, key, &do_nothing, &is_valid_token_character, lexer_);
    if ('=' != character) {
        return false;
    }
    character = lexer_.get();
    parse_word(character, value, &do_nothing, &is_valid_token_character, lexer_);
    character = lexer_.get();
    parameters_[key.c_str()] = value.c_str();
    return true;
}

} // namespace http

} // namespace rest
