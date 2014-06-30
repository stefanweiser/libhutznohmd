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

#include <http/parser/utility/uri.hpp>

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
    std::unique_ptr<uri> parse(const bool expect_sucess = true);

    std::string str_;
};

fixture::fixture(const std::string & str)
    : str_(str)
{}

std::unique_ptr<uri> fixture::parse(const bool expect_sucess)
{
    string_index_pair p(str_, 0);
    lexer l(anonymous_int_function(&get_char, &p),
            anonymous_int_function(&peek_char, &p));
    std::unique_ptr<uri> m = std::unique_ptr<uri>(new uri(l));
    int32_t result = l.get();
    EXPECT_EQ(expect_sucess, m->parse(result));
    return m;
}

} // namespace

TEST(uri, empty_then_set_scheme)
{
    fixture f("/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::UNKNOWN);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/"));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));

    u->set_scheme(uri_scheme::MAILTO);

    EXPECT_EQ(u->scheme(), uri_scheme::MAILTO);
}

TEST(uri, http_localhost)
{
    fixture f("http://localhost/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string("localhost"));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/"));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, complete_uri)
{
    fixture f("http://user:password@localhost:80/?a=b#anchor");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string("user:password"));
    EXPECT_EQ(u->host().c_str(), std::string("localhost"));
    EXPECT_EQ(u->port(), 80);
    EXPECT_EQ(u->path().c_str(), std::string("/"));
    EXPECT_EQ(u->query().c_str(), std::string("a=b"));
    EXPECT_EQ(u->fragment().c_str(), std::string("anchor"));
}

TEST(uri, erroneous_port)
{
    fixture f("http://localhost:80000/");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string("localhost:80000"));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, mailto_user_at_localhost)
{
    fixture f("mailto://user@localhost");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::MAILTO);
    EXPECT_EQ(u->userinfo().c_str(), std::string("user"));
    EXPECT_EQ(u->host().c_str(), std::string("localhost"));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, no_authority)
{
    fixture f("http:/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/"));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, query_only)
{
    fixture f("http:?a=b");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string("a=b"));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, fragment_only)
{
    fixture f("http:#anchor");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string("anchor"));
}

TEST(uri, no_scheme_no_authority)
{
    fixture f("/");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::UNKNOWN);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/"));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, http_localhost_with_percent_encoding)
{
    fixture f("http://localhost/%48%65%6c%6C%6f%20%57%6F%72%6c%64%21");
    const std::unique_ptr<uri> u = f.parse();
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string("localhost"));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/Hello World!"));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, http_localhost_with_erroneous_percent_encoding)
{
    fixture f("http://localhost/%48%65%6g%6C%6f%20%57%6F%72%6c%64%21");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string("localhost"));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/He"));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, http_localhost_with_erroneous_percent_encoding2)
{
    fixture f("http://localhost/%48%65%6");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string("localhost"));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/He"));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, erroneous_scheme)
{
    fixture f("html");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::UNKNOWN);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, erroneous_scheme2)
{
    fixture f("http//");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, erroneous_scheme3)
{
    fixture f("http:");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, erroneous_query)
{
    fixture f("/?a%2");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::UNKNOWN);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/"));
    EXPECT_EQ(u->query().c_str(), std::string("a"));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, erroneous_fragment)
{
    fixture f("/#a%2");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::UNKNOWN);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string(""));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string("/"));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string("a"));
}

TEST(uri, erroneous_authority1)
{
    fixture f("http://loca%2lhost/");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string(""));
    EXPECT_EQ(u->host().c_str(), std::string("loca"));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

TEST(uri, erroneous_authority2)
{
    fixture f("http://user@loca%2lhost/");
    const std::unique_ptr<uri> u = f.parse(false);
    EXPECT_EQ(u->scheme(), uri_scheme::HTTP);
    EXPECT_EQ(u->userinfo().c_str(), std::string("user"));
    EXPECT_EQ(u->host().c_str(), std::string("loca"));
    EXPECT_EQ(u->port(), 0);
    EXPECT_EQ(u->path().c_str(), std::string(""));
    EXPECT_EQ(u->query().c_str(), std::string(""));
    EXPECT_EQ(u->fragment().c_str(), std::string(""));
}

} // namespace http

} // namespace rest
