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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include <http/parser/utility/mediatype.hpp>

using namespace testing;

namespace rest
{

namespace http
{

namespace
{

typedef std::pair<std::string, size_t> string_index_pair;

int32_t get_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second++]);
    }
    return -1;
}

int32_t peek_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return static_cast<uint8_t>(p->first[p->second]);
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string & str);
    media_type parse(const bool expect_sucess = true);

    std::string str_;
};

fixture::fixture(const std::string & str)
    : str_(str)
{}

media_type fixture::parse(const bool expect_sucess)
{
    string_index_pair p(str_, 0);
    lexer l(anonymous_int_function(&get_char, &p),
            anonymous_int_function(&peek_char, &p));
    media_type m;
    int32_t result = l.get();
    EXPECT_EQ(expect_sucess, m.parse(l, result));
    return m;
}

} // namespace

TEST(http_media_type, empty_wildcard)
{
    fixture f("/");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::CUSTOM);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, wildcard_wildcard)
{
    fixture f("*/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::WILDCARD);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, application_wildcard)
{
    fixture f("application/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::APPLICATION);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, audio_bla)
{
    fixture f("audio/*bla");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::AUDIO);
    EXPECT_EQ(m.subtype(), media_type_subtype::CUSTOM);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string("*bla"));
}

TEST(http_media_type, example_wildcard)
{
    fixture f("example/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::EXAMPLE);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, image_wildcard)
{
    fixture f("image/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::IMAGE);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, message_wildcard)
{
    fixture f("message/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::MESSAGE);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, model_wildcard)
{
    fixture f("model/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::MODEL);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, multipart_wildcard)
{
    fixture f("multipart/*;r=\"\xFF\xFE\xFD\"");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::MULTIPART);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
    EXPECT_EQ(m.parameters().find("r")->second, std::string("\xFF\xFE\xFD"));
}

TEST(http_media_type, text_wildcard)
{
    fixture f("text/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::TEXT);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, video_wildcard)
{
    fixture f("video/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::VIDEO);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, text_plain_parameter)
{
    fixture f("text/plain;r=0.1");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::TEXT);
    EXPECT_EQ(m.subtype(), media_type_subtype::PLAIN);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
    EXPECT_EQ(m.parameters().find("p"), m.parameters().end());
    EXPECT_EQ(m.parameters().find("r")->second, std::string("0.1"));
    EXPECT_EQ(m.quality(), 10);
}

TEST(http_media_type, applucation_wildcard)
{
    fixture f("applucation/*;p=0.1;q=0.5;r=0.2");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string("applucation"));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
    EXPECT_EQ(m.parameters().find("p")->second, std::string("0.1"));
    EXPECT_EQ(m.parameters().find("q"), m.parameters().end());
    EXPECT_EQ(m.parameters().find("r")->second, std::string("0.2"));
    EXPECT_EQ(m.quality(), 5);
}

TEST(http_media_type, audo_wildcard)
{
    fixture f("audo/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string("audo"));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, examle_wildcard)
{
    fixture f("examle/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string("examle"));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, imag_wildcard)
{
    fixture f("imag/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string("imag"));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, mess_wildcard)
{
    fixture f("mess/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string("mess"));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, m_wildcard)
{
    fixture f("m/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string("m"));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, x_wildcard)
{
    fixture f("x/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string("x"));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, starm_wildcard)
{
    fixture f("*m/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.type(), media_type_type::CUSTOM);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string("*m"));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, erroneous_slash)
{
    fixture f("audio,*");
    const media_type m = f.parse(false);
    EXPECT_EQ(m.type(), media_type_type::AUDIO);
    EXPECT_EQ(m.subtype(), media_type_subtype::CUSTOM);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
}

TEST(http_media_type, erroneous_parameter_equal_sign)
{
    fixture f("audio/*;q,0.1");
    const media_type m = f.parse(false);
    EXPECT_EQ(m.type(), media_type_type::AUDIO);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
    EXPECT_EQ(m.parameters().find("q"), m.parameters().end());
}

TEST(http_media_type, erroneous_parameter_quoted_string)
{
    fixture f("audio/*;q=\"0.1\n");
    const media_type m = f.parse(false);
    EXPECT_EQ(m.type(), media_type_type::AUDIO);
    EXPECT_EQ(m.subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m.custom_type(), std::string(""));
    EXPECT_EQ(m.custom_subtype(), std::string(""));
    EXPECT_EQ(m.parameters().find("q"), m.parameters().end());
}

TEST(http_media_type, specification_grade1)
{
    fixture f("*/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.specification_grade(), 0);
}

TEST(http_media_type, specification_grade2)
{
    fixture f("*/html");
    const media_type m = f.parse();
    EXPECT_EQ(m.specification_grade(), 1);
}

TEST(http_media_type, specification_grade3)
{
    fixture f("text/*");
    const media_type m = f.parse();
    EXPECT_EQ(m.specification_grade(), 2);
}

TEST(http_media_type, specification_grade4)
{
    fixture f("text/html");
    const media_type m = f.parse();
    EXPECT_EQ(m.specification_grade(), 3);
}

} // namespace http

} // namespace rest
