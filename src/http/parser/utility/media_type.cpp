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

#include <array>

#include <http/parser/utility/character_handling.hpp>
#include <http/parser/utility/trie.hpp>

#include "media_type.hpp"

namespace hutzn
{

namespace http
{

media_type::media_type()
    : lexer_(nullptr)
    , type_(media_type_interface::mime_type::CUSTOM)
    , subtype_(media_type_interface::mime_subtype::CUSTOM)
    , custom_type_()
    , custom_subtype_()
    , parameters_()
    , quality_(10)
{
}

media_type::media_type(const media_type& rhs)
    : lexer_(rhs.lexer_)
    , type_(rhs.type_)
    , subtype_(rhs.subtype_)
    , custom_type_()
    , custom_subtype_()
    , parameters_(rhs.parameters_)
    , quality_(rhs.quality_)
{
    custom_type_.append_string(rhs.custom_type_.c_str());
    custom_subtype_.append_string(rhs.custom_subtype_.c_str());
}

media_type& media_type::operator=(const media_type& rhs)
{
    lexer_ = rhs.lexer_;
    type_ = rhs.type_;
    subtype_ = rhs.subtype_;
    custom_type_.clear();
    custom_type_.append_string(rhs.custom_type_.c_str());
    custom_subtype_.clear();
    custom_subtype_.append_string(rhs.custom_subtype_.c_str());
    parameters_ = rhs.parameters_;
    quality_ = rhs.quality_;
    return *this;
}

bool media_type::parse(const lexer& l, int32_t& character)
{
    lexer_ = &l;

    parse_type(character);
    if ('/' != character) {
        return false;
    }
    character = lexer_->get_non_whitespace();
    parse_subtype(character);
    while (';' == character) {
        character = lexer_->get_non_whitespace();
        if (false == parse_parameter(character)) {
            return false;
        }
    }
    return true;
}

media_type_interface::mime_type media_type::type() const
{
    return type_;
}

media_type_interface::mime_subtype media_type::subtype() const
{
    return subtype_;
}

const char* media_type::custom_type() const
{
    return custom_type_.c_str();
}

const char* media_type::custom_subtype() const
{
    return custom_subtype_.c_str();
}

const std::map<std::string, std::string>& media_type::parameters() const
{
    return parameters_;
}

uint8_t media_type::quality() const
{
    return quality_;
}

uint8_t media_type::specification_grade() const
{
    if (media_type_interface::mime_type::WILDCARD == type_) {
        if (media_type_interface::mime_subtype::WILDCARD == subtype_) {
            return 0;
        } else {
            return 1;
        }
    } else {
        if (media_type_interface::mime_subtype::WILDCARD == subtype_) {
            return 2;
        } else {
            return 3;
        }
    }
}

void media_type::parse_type(int32_t& character)
{
    using value_info = trie<media_type_interface::mime_type>::value_info;

    // The index of the vector has to be the same as the value of the type,
    // because this largely
    // boosts the conversion below.
    static const std::vector<value_info> types = {
        {value_info{"*", media_type_interface::mime_type::WILDCARD},
         value_info{"application",
                    media_type_interface::mime_type::APPLICATION},
         value_info{"audio", media_type_interface::mime_type::AUDIO},
         value_info{"example", media_type_interface::mime_type::EXAMPLE},
         value_info{"image", media_type_interface::mime_type::IMAGE},
         value_info{"message", media_type_interface::mime_type::MESSAGE},
         value_info{"model", media_type_interface::mime_type::MODEL},
         value_info{"multipart", media_type_interface::mime_type::MULTIPART},
         value_info{"text", media_type_interface::mime_type::TEXT},
         value_info{"video", media_type_interface::mime_type::VIDEO}}};

    static const trie<media_type_interface::mime_type> t(
        types, media_type_interface::mime_type::CUSTOM);
    type_ = t.parse(character, custom_type_, *lexer_);
    if ((media_type_interface::mime_type::CUSTOM != type_) &&
        (true == is_valid_token_character(static_cast<char>(character)))) {
        custom_type_.append_string(
            std::get<0>(types[static_cast<int32_t>(type_)]));
        type_ = media_type_interface::mime_type::CUSTOM;
    }

    if (media_type_interface::mime_type::CUSTOM == type_) {
        parse_word(character, custom_type_, &is_valid_token_character, *lexer_);
    }
}

void media_type::parse_subtype(int32_t& character)
{
    using value_info = trie<media_type_interface::mime_subtype>::value_info;

    // The index of the vector has to be the same as the value of the subtype,
    // because this
    // largely boosts the conversion below.
    static const std::vector<value_info> types = {
        {value_info{"*", media_type_interface::mime_subtype::WILDCARD},
         value_info{"plain", media_type_interface::mime_subtype::PLAIN}}};

    static const trie<media_type_interface::mime_subtype> t(
        types, media_type_interface::mime_subtype::CUSTOM);
    subtype_ = t.parse(character, custom_subtype_, *lexer_);
    if ((media_type_interface::mime_subtype::CUSTOM != subtype_) &&
        (true == is_valid_token_character(static_cast<char>(character)))) {
        custom_subtype_.append_string(
            std::get<0>(types[static_cast<int32_t>(subtype_)]));
        subtype_ = media_type_interface::mime_subtype::CUSTOM;
    }

    if (media_type_interface::mime_subtype::CUSTOM == subtype_) {
        parse_word(character, custom_subtype_, &is_valid_token_character,
                   *lexer_);
    }
}

bool media_type::parse_parameter(int32_t& character)
{
    using parsing_function = bool (media_type::*)(int32_t&);
    using trie_value = std::tuple<parsing_function, size_t>;
    using value_info = trie<trie_value>::value_info;

    // The index of the vector has to be the same as the value of the type,
    // because this largely
    // boosts the conversion below.
    static const std::vector<value_info> types = {
        {value_info{"q", trie_value{&media_type::parse_quality_parameter, 0}}}};

    push_back_string<16> key;
    push_back_string<16> value;
    static const trie<trie_value> t(types, trie_value{nullptr, -1});
    trie_value v = t.parse(character, key, *lexer_);
    if ('=' == character) {
        character = lexer_->get_non_whitespace();
        parsing_function functor = std::get<0>(v);
        if (nullptr != functor) {
            return (this->*functor)(character);
        }
    }

    size_t index = std::get<1>(v);
    if (index < types.size()) {
        key.append_string(std::get<0>(types[index]));
    }

    parse_word(character, key, &is_valid_token_character, *lexer_);

    if ('=' != character) {
        return false;
    }
    character = lexer_->get();
    if ('"' == character) {
        if (false == parse_quoted_string(character, value, *lexer_)) {
            return false;
        }
    } else {
        parse_word(character, value, &is_valid_token_character, *lexer_);
    }
    parameters_[key.c_str()] = value.c_str();
    return true;
}

bool media_type::parse_quality_parameter(int32_t& character)
{
    const uint8_t upper = static_cast<uint8_t>(character - '0');
    if (upper > 1) {
        return false;
    }

    character = lexer_->get();
    if ('.' != character) {
        return false;
    }

    character = lexer_->get();
    const uint8_t lower = static_cast<uint8_t>(character - '0');
    if (lower > 9) {
        return false;
    }

    quality_ = static_cast<uint8_t>((upper * 10) + lower);
    character = lexer_->get();
    return true;
}

} // namespace http

} // namespace hutzn
