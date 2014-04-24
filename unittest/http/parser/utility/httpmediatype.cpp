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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#define private public
#define protected public

#include <http/parser/utility/httpmediatype.hpp>

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
        return p->first[p->second++];
    }
    return -1;
}

int32_t peek_char(void * handle)
{
    string_index_pair * p = static_cast<string_index_pair *>(handle);
    if (p->second < p->first.size()) {
        return p->first[p->second];
    }
    return -1;
}

class fixture
{
public:
    explicit fixture(const std::string & str);
    std::unique_ptr<media_type> parse();

    std::string str_;
};

fixture::fixture(const std::string & str)
    : str_(str)
{}

std::unique_ptr<media_type> fixture::parse()
{
    string_index_pair p(str_, 0);
    lexer l(anonymous_int_function(&get_char, &p),
            anonymous_int_function(&peek_char, &p));
    std::unique_ptr<media_type> m = std::unique_ptr<media_type>(new media_type(l));
    int32_t result = l.get();
    m->parse(result);
    return m;
}

} // namespace

TEST(http_media_type, empty_wildcard)
{
    fixture f("/");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::CUSTOM);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, wildcard_wildcard)
{
    fixture f("*/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::WILDCARD);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, application_wildcard)
{
    fixture f("application/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::APPLICATION);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, audio_wildcard)
{
    fixture f("audio/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::AUDIO);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, example_wildcard)
{
    fixture f("example/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::EXAMPLE);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, image_wildcard)
{
    fixture f("image/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::IMAGE);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, message_wildcard)
{
    fixture f("message/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::MESSAGE);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, model_wildcard)
{
    fixture f("model/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::MODEL);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, multipart_wildcard)
{
    fixture f("multipart/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::MULTIPART);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, text_wildcard)
{
    fixture f("text/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::TEXT);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, video_wildcard)
{
    fixture f("video/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::VIDEO);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, text_plain_parameter)
{
    fixture f("text/plain;q=0.1");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::TEXT);
    EXPECT_EQ(m->subtype(), media_type_subtype::PLAIN);
    EXPECT_EQ(m->custom_type(), std::string(""));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
    EXPECT_EQ(m->parameter("q"), std::string("0.1"));
}

TEST(http_media_type, applucation_wildcard)
{
    fixture f("applucation/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string("applucation"));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, audo_wildcard)
{
    fixture f("audo/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string("audo"));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, examle_wildcard)
{
    fixture f("examle/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string("examle"));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, imag_wildcard)
{
    fixture f("imag/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string("imag"));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, mess_wildcard)
{
    fixture f("mess/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string("mess"));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, m_wildcard)
{
    fixture f("m/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string("m"));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, x_wildcard)
{
    fixture f("x/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string("x"));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

TEST(http_media_type, starm_wildcard)
{
    fixture f("*m/*");
    const std::unique_ptr<media_type> m = f.parse();
    EXPECT_EQ(m->type(), media_type_type::CUSTOM);
    EXPECT_EQ(m->subtype(), media_type_subtype::WILDCARD);
    EXPECT_EQ(m->custom_type(), std::string("*m"));
    EXPECT_EQ(m->custom_subtype(), std::string(""));
}

} // namespace http

} // namespace rest