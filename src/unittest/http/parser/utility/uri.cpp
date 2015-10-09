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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include <http/parser/utility/uri.hpp>

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
    std::unique_ptr<uri> parse(const bool expect_sucess = true);

    std::string str_;
};

fixture::fixture(const std::string& str)
    : str_(str)
{
}

std::unique_ptr<uri> fixture::parse(const bool expect_sucess)
{
    string_index_pair p(str_, 0);
    lexer l(anonymous_int_function(&get_char, &p),
            anonymous_int_function(&peek_char, &p));
    std::unique_ptr<uri> m = std::unique_ptr<uri>(new uri());
    int32_t result = l.get();
    EXPECT_EQ(expect_sucess, m->parse(l, result, false));
    return m;
}

} // namespace

TEST(uri, empty_then_set_scheme)
{
    fixture f("/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());

    u->set_scheme(uri_scheme::MAILTO);

    EXPECT_EQ(uri_scheme::MAILTO, u->scheme());
}

TEST(uri, empty_then_set_host)
{
    fixture f("/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());

    u->set_host("localhost");

    EXPECT_EQ(std::string("localhost"), u->host());
}

TEST(uri, empty_then_set_userinfo)
{
    fixture f("/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());

    u->set_userinfo("user:pw");

    EXPECT_EQ(std::string("user:pw"), u->userinfo());
}

TEST(uri, empty_then_set_port)
{
    fixture f("/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());

    u->set_port(88);

    EXPECT_EQ(88, u->port());
}

TEST(uri, http_localhost)
{
    fixture f("http://localhost/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, complete_uri)
{
    fixture f("http://user:password@localhost:80/?a=b#anchor");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string("user:password"), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string("a=b"), u->query());
    EXPECT_EQ(std::string("anchor"), u->fragment());
}

TEST(uri, erroneous_port)
{
    fixture f("http://localhost:80000/");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost:80000"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, mailto_user_at_localhost)
{
    fixture f("mailto://user@localhost");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::MAILTO, u->scheme());
    EXPECT_EQ(std::string("user"), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, no_authority)
{
    fixture f("http:/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, query_only)
{
    fixture f("http:?a=b");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string("a=b"), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, fragment_only)
{
    fixture f("http:#anchor");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string("anchor"), u->fragment());
}

TEST(uri, no_scheme_no_authority)
{
    fixture f("/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, http_localhost_with_percent_encoding)
{
    fixture f("http://localhost/%48%65%6c%6C%6f%20%57%6F%72%6c%64%21");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/Hello World!"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, http_localhost_with_erroneous_percent_encoding)
{
    fixture f("http://localhost/%48%65%6g%6C%6f%20%57%6F%72%6c%64%21");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/He"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, http_localhost_with_erroneous_percent_encoding2)
{
    fixture f("http://localhost/%48%65%6");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/He"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, erroneous_scheme)
{
    fixture f("html");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, erroneous_scheme2)
{
    fixture f("http//");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, erroneous_scheme3)
{
    fixture f("http:");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, erroneous_query)
{
    fixture f("/?a%2");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(u->path(), std::string("/"));
    EXPECT_EQ(u->query(), std::string("a"));
    EXPECT_EQ(u->fragment(), std::string(""));
}

TEST(uri, erroneous_fragment)
{
    fixture f("/#a%2");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(u->path(), std::string("/"));
    EXPECT_EQ(u->query(), std::string(""));
    EXPECT_EQ(u->fragment(), std::string("a"));
}

TEST(uri, erroneous_authority1)
{
    fixture f("http://loca%2lhost/");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("loca"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST(uri, erroneous_authority2)
{
    fixture f("http://user@loca%2lhost/");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string("user"), u->userinfo());
    EXPECT_EQ(std::string("loca"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

} // namespace http

} // namespace hutzn