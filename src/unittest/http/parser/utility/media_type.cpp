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

#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "http/parser/utility/media_type.hpp"

using namespace testing;

namespace hutzn
{

namespace http
{

namespace
{

using string_index_pair = std::pair<std::string, size_t>;

int32_t get_char(void* handle)
{
    string_index_pair* p = static_cast<string_index_pair*>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second++]);
    }
    return -1;
}

int32_t peek_char(void* handle)
{
    string_index_pair* p = static_cast<string_index_pair*>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second]);
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string& str);
    media_type parse(const bool expect_sucess = true);

    std::string str_;
};

fixture::fixture(const std::string& str)
    : str_(str)
{
}

media_type fixture::parse(const bool expect_sucess)
{
    string_index_pair p(str_, 0);
    lexer lex(anonymous_int_function(&get_char, &p),
              anonymous_int_function(&peek_char, &p));
    media_type m;
    int32_t result = lex.get();
    EXPECT_EQ(expect_sucess, m.parse(lex, result));
    return m;
}

} // namespace

TEST(media_type, empty_wildcard)
{
    fixture f("/");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::CUSTOM, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, copy_wildcard)
{
    fixture f("*/*");
    const media_type m = f.parse();
    media_type n;
    n = m;
    EXPECT_EQ(m.type(), n.type());
    EXPECT_EQ(m.subtype(), n.subtype());
    EXPECT_EQ(std::string(m.custom_type()), n.custom_type());
    EXPECT_EQ(std::string(m.custom_subtype()), n.custom_subtype());
    EXPECT_EQ(m.quality(), n.quality());
    EXPECT_EQ(m.parameters(), n.parameters());
    EXPECT_EQ(m.specification_grade(), n.specification_grade());
}

TEST(media_type, wildcard_wildcard)
{
    fixture f("*/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::WILDCARD, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, application_wildcard)
{
    fixture f("application/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::APPLICATION, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, audio_bla)
{
    fixture f("audio/*bla");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::AUDIO, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::CUSTOM, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string("*bla"), m.custom_subtype());
}

TEST(media_type, example_wildcard)
{
    fixture f("example/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::EXAMPLE, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, image_wildcard)
{
    fixture f("image/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::IMAGE, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, message_wildcard)
{
    fixture f("message/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::MESSAGE, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, model_wildcard)
{
    fixture f("model/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::MODEL, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, multipart_wildcard)
{
    fixture f("multipart/*;r=\"\xFF\xFE\xFD\"");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::MULTIPART, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
    EXPECT_EQ(std::string("\xFF\xFE\xFD"), m.parameters().find("r")->second);
}

TEST(media_type, text_wildcard)
{
    fixture f("text/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::TEXT, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, video_wildcard)
{
    fixture f("video/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::VIDEO, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, text_plain_parameter)
{
    fixture f("text/plain;r=0.1");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::TEXT, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::PLAIN, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
    EXPECT_EQ(m.parameters().end(), m.parameters().find("p"));
    EXPECT_EQ(std::string("0.1"), m.parameters().find("r")->second);
    EXPECT_EQ(10, m.quality());
}

TEST(media_type, applucation_wildcard)
{
    fixture f("applucation/*;p=0.1;q=0.5;r=0.2");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string("applucation"), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
    EXPECT_EQ(std::string("0.1"), m.parameters().find("p")->second);
    EXPECT_EQ(m.parameters().end(), m.parameters().find("q"));
    EXPECT_EQ(std::string("0.2"), m.parameters().find("r")->second);
    EXPECT_EQ(5, m.quality());
}

TEST(media_type, audo_wildcard)
{
    fixture f("audo/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string("audo"), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, examle_wildcard)
{
    fixture f("examle/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string("examle"), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, imag_wildcard)
{
    fixture f("imag/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string("imag"), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, mess_wildcard)
{
    fixture f("mess/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string("mess"), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, m_wildcard)
{
    fixture f("m/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string("m"), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, x_wildcard)
{
    fixture f("x/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string("x"), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, starm_wildcard)
{
    fixture f("*m/*");
    const media_type m = f.parse();
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string("*m"), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, erroneous_slash)
{
    fixture f("audio,*");
    const media_type m = f.parse(false);
    EXPECT_EQ(media_type_interface::mime_type::AUDIO, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::CUSTOM, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
}

TEST(media_type, erroneous_parameter_equal_sign)
{
    fixture f("audio/*;q,0.1");
    const media_type m = f.parse(false);
    EXPECT_EQ(media_type_interface::mime_type::AUDIO, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
    EXPECT_EQ(m.parameters().end(), m.parameters().find("q"));
}

TEST(media_type, erroneous_parameter_quoted_string1)
{
    fixture f("audio/*;q=\"0.1\n");
    const media_type m = f.parse(false);
    EXPECT_EQ(media_type_interface::mime_type::AUDIO, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
    EXPECT_EQ(m.parameters().end(), m.parameters().find("q"));
}

TEST(media_type, erroneous_parameter_quoted_string2)
{
    fixture f("audio/*;r=\"");
    const media_type m = f.parse(false);
    EXPECT_EQ(media_type_interface::mime_type::AUDIO, m.type());
    EXPECT_EQ(media_type_interface::mime_subtype::WILDCARD, m.subtype());
    EXPECT_EQ(std::string(""), m.custom_type());
    EXPECT_EQ(std::string(""), m.custom_subtype());
    EXPECT_EQ(m.parameters().end(), m.parameters().find("r"));
}

TEST(media_type, erroneous_quality_parameter1)
{
    fixture f("audio/*;q=0");
    const media_type m = f.parse(false);
    EXPECT_EQ(10, m.quality());
}

TEST(media_type, erroneous_quality_parameter2)
{
    fixture f("audio/*;q=0.a");
    const media_type m = f.parse(false);
    EXPECT_EQ(10, m.quality());
}

TEST(media_type, specification_grade1)
{
    fixture f("*/*");
    const media_type m = f.parse();
    EXPECT_EQ(0, m.specification_grade());
}

TEST(media_type, specification_grade2)
{
    fixture f("*/html");
    const media_type m = f.parse();
    EXPECT_EQ(1, m.specification_grade());
}

TEST(media_type, specification_grade3)
{
    fixture f("text/*");
    const media_type m = f.parse();
    EXPECT_EQ(2, m.specification_grade());
}

TEST(media_type, specification_grade4)
{
    fixture f("text/html");
    const media_type m = f.parse();
    EXPECT_EQ(3, m.specification_grade());
}

} // namespace http

} // namespace hutzn
