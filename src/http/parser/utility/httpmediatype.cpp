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

#include <array>

#include <http/parser/utility/charactercompare.hpp>

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

using parsing_function_type = void(media_type::*)(int32_t &);

void media_type::parse_type_end(int32_t & /*character*/)
{}

void media_type::parse_type_custom(int32_t & character)
{
    while (true == is_valid_token_character(static_cast<uint8_t>(character))) {
        custom_type_.push_back(to_lower(static_cast<char>(character)));
        character = lexer_.get();
    }
}

bool media_type::compare_type(int32_t & character,
                              const char * ref,
                              const size_t & already_parsed)
{
    return compare_to_reference(character,
                                ref,
                                already_parsed,
                                custom_type_,
                                &is_valid_token_character,
                                lexer_);
}

void media_type::parse_type_wildcard(int32_t & character)
{
    if (true == compare_type(character, "*", 1)) {
        type_ = media_type_type::WILDCARD;
    }
}

void media_type::parse_type_application(int32_t & character)
{
    if (true == compare_type(character, "application", 2)) {
        type_ = media_type_type::APPLICATION;
    }
}

void media_type::parse_type_audio(int32_t & character)
{
    if (true == compare_type(character, "audio", 2)) {
        type_ = media_type_type::AUDIO;
    }
}

void media_type::parse_type_example(int32_t & character)
{
    if (true == compare_type(character, "example", 1)) {
        type_ = media_type_type::EXAMPLE;
    }
}

void media_type::parse_type_image(int32_t & character)
{
    if (true == compare_type(character, "image", 1)) {
        type_ = media_type_type::IMAGE;
    }
}

void media_type::parse_type_message(int32_t & character)
{
    if (true == compare_type(character, "message", 2)) {
        type_ = media_type_type::MESSAGE;
    }
}

void media_type::parse_type_model(int32_t & character)
{
    if (true == compare_type(character, "model", 2)) {
        type_ = media_type_type::MODEL;
    }
}

void media_type::parse_type_multipart(int32_t & character)
{
    if (true == compare_type(character, "multipart", 2)) {
        type_ = media_type_type::MULTIPART;
    }
}

void media_type::parse_type_text(int32_t & character)
{
    if (true == compare_type(character, "text", 1)) {
        type_ = media_type_type::TEXT;
    }
}

void media_type::parse_type_video(int32_t & character)
{
    if (true == compare_type(character, "video", 1)) {
        type_ = media_type_type::VIDEO;
    }
}

void media_type::parse_type_a(int32_t & character)
{
    using parsing_function_map = std::array<parsing_function_type, 128>;
    static const auto generate_type_character = []() -> std::array<parsing_function_type, 128> {
        parsing_function_map result;
        for (size_t i = 0; i < result.size(); i++) {
            bool valid = is_valid_token_character(static_cast<uint8_t>(i));
            result[i] = valid ? &media_type::parse_type_custom : &media_type::parse_type_end;
        }

        result['P'] = &media_type::parse_type_application;
        result['p'] = &media_type::parse_type_application;
        result['U'] = &media_type::parse_type_audio;
        result['u'] = &media_type::parse_type_audio;
        return result;
    };
    static const parsing_function_map type_character = generate_type_character();

    character = lexer_.get();
    if (((static_cast<uint32_t>(character) & 0xFFFFFF80) != 0) ||
        (false == is_valid_token_character(static_cast<uint8_t>(character)))) {
        custom_type_.push_back('a');
        return;
    }
    auto pointer = type_character[static_cast<uint8_t>(character)];
    (this->*pointer)(character);
}

void media_type::parse_type_m(int32_t & character)
{
    using parsing_function_map = std::array<parsing_function_type, 128>;
    static const auto generate_type_character = []() -> std::array<parsing_function_type, 128> {
        parsing_function_map result;
        for (size_t i = 0; i < result.size(); i++) {
            bool valid = is_valid_token_character(static_cast<uint8_t>(i));
            result[i] = valid ? &media_type::parse_type_custom : &media_type::parse_type_end;
        }

        result['E'] = &media_type::parse_type_message;
        result['e'] = &media_type::parse_type_message;
        result['O'] = &media_type::parse_type_model;
        result['o'] = &media_type::parse_type_model;
        result['U'] = &media_type::parse_type_multipart;
        result['u'] = &media_type::parse_type_multipart;
        return result;
    };
    static const parsing_function_map type_character = generate_type_character();

    character = lexer_.get();
    if (((static_cast<uint32_t>(character) & 0xFFFFFF80) != 0) ||
        (false == is_valid_token_character(static_cast<uint8_t>(character)))) {
        custom_type_.push_back('m');
        return;
    }
    auto pointer = type_character[static_cast<uint8_t>(character)];
    (this->*pointer)(character);
}

void media_type::parse_type(int32_t & character)
{
    using parsing_function_map = std::array<parsing_function_type, 128>;
    static const auto generate_type_character = []() -> std::array<parsing_function_type, 128> {
        parsing_function_map result;
        for (size_t i = 0; i < result.size(); i++) {
            bool valid = is_valid_token_character(static_cast<uint8_t>(i));
            result[i] = valid ? &media_type::parse_type_custom : &media_type::parse_type_end;
        }

        result['A'] = &media_type::parse_type_a;
        result['a'] = &media_type::parse_type_a;
        result['E'] = &media_type::parse_type_example;
        result['e'] = &media_type::parse_type_example;
        result['M'] = &media_type::parse_type_m;
        result['m'] = &media_type::parse_type_m;
        result['I'] = &media_type::parse_type_image;
        result['i'] = &media_type::parse_type_image;
        result['T'] = &media_type::parse_type_text;
        result['t'] = &media_type::parse_type_text;
        result['V'] = &media_type::parse_type_video;
        result['v'] = &media_type::parse_type_video;
        result['*'] = &media_type::parse_type_wildcard;
        return result;
    };
    static const parsing_function_map type_character = generate_type_character();

    if (((static_cast<uint32_t>(character) & 0xFFFFFF80) != 0) ||
        (false == is_valid_token_character(static_cast<uint8_t>(character)))) {
        return;
    }
    auto pointer = type_character[static_cast<uint8_t>(character)];
    (this->*pointer)(character);
}

bool media_type::compare_subtype(int32_t & character,
                                 const char * ref,
                                 const size_t & already_parsed)
{
    return compare_to_reference(character,
                                ref,
                                already_parsed,
                                custom_subtype_,
                                &is_valid_token_character,
                                lexer_);
}

void media_type::parse_subtype_wildcard(int32_t & character)
{
    if (true == compare_subtype(character, "*", 1)) {
        subtype_ = media_type_subtype::WILDCARD;
    }
}

void media_type::parse_subtype_plain(int32_t & character)
{
    if (true == compare_subtype(character, "plain", 1)) {
        subtype_ = media_type_subtype::PLAIN;
    }
}

void media_type::parse_subtype(int32_t & character)
{
    using parsing_function_map = std::array<parsing_function_type, 128>;
    static const auto generate_type_character = []() -> std::array<parsing_function_type, 128> {
        parsing_function_map result;
        for (size_t i = 0; i < result.size(); i++) {
            bool valid = is_valid_token_character(static_cast<uint8_t>(i));
            result[i] = valid ? &media_type::parse_type_custom : &media_type::parse_type_end;
        }

        result['P'] = &media_type::parse_subtype_plain;
        result['p'] = &media_type::parse_subtype_plain;
        result['*'] = &media_type::parse_subtype_wildcard;
        return result;
    };
    static const parsing_function_map type_character = generate_type_character();

    if (((static_cast<uint32_t>(character) & 0xFFFFFF80) != 0) ||
        (false == is_valid_token_character(static_cast<uint8_t>(character)))) {
        return;
    }
    auto pointer = type_character[static_cast<uint8_t>(character)];
    (this->*pointer)(character);
}

bool media_type::parse_parameter(int32_t & character)
{
    push_back_string<16> key;
    push_back_string<16> value;

    character = lexer_.get_non_whitespace();
    auto nothing = [](const char & c) {
        return c;
    };

    parse_word(character, key, nothing, &is_valid_token_character, lexer_);
    if ('=' != character) {
        return false;
    }
    character = lexer_.get();
    parse_word(character, value, nothing, &is_valid_token_character, lexer_);
    character = lexer_.get();
    parameters_[key.c_str()] = value.c_str();
    return true;
}

} // namespace http

} // namespace rest
