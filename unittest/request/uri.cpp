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

#include <request/uri.hpp>

using namespace testing;

namespace hutzn
{

class uri_test : public ::testing::Test
{
public:
    uri::first_pass_data check_1st_pass(std::string& str,
                                        const bool expect_sucess,
                                        const bool skip_scheme = false)
    {
        uri u;
        size_t s = str.size();
        char_t* ptr = &(str[0]);
        uri::first_pass_data data;
        EXPECT_EQ(expect_sucess, u.parse_1st_pass(ptr, s, data, skip_scheme));
        return data;
    }

    std::unique_ptr<uri> check_parse(std::string& str,
                                     const bool expect_sucess = true,
                                     const bool skip_scheme = false)
    {
        std::unique_ptr<uri> u = std::unique_ptr<uri>(new uri());
        char_t* ptr = &(str[0]);
        size_t size = str.size();
        EXPECT_EQ(expect_sucess, u->parse(ptr, size, skip_scheme));
        return u;
    }
};

TEST_F(uri_test, first_pass_nullptr)
{
    uri u;
    char_t* ptr = nullptr;
    size_t s = 0;
    uri::first_pass_data data;
    EXPECT_TRUE(u.parse_1st_pass(ptr, s, data, false));
    EXPECT_STREQ(nullptr, data.scheme);
    EXPECT_STREQ(nullptr, data.authority);
    EXPECT_STREQ(nullptr, data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_empty)
{
    std::string str = "";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ(nullptr, data.scheme);
    EXPECT_STREQ(nullptr, data.authority);
    EXPECT_STREQ(nullptr, data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_simple_path)
{
    std::string str = "/";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ(nullptr, data.scheme);
    EXPECT_STREQ(nullptr, data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_localhost)
{
    std::string str = "http://localhost/";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("localhost", data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(9, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_complete_uri)
{
    std::string str = "http://user:password@localhost:80/?a=b#anchor";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_authority)
{
    std::string str = "localhost";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ(nullptr, data.scheme);
    EXPECT_STREQ("localhost", data.authority);
    EXPECT_STREQ(nullptr, data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(9, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_authority_with_userinfo)
{
    std::string str = "user:password@localhost";
    const uri::first_pass_data data = check_1st_pass(str, true, true);
    EXPECT_STREQ(nullptr, data.scheme);
    EXPECT_STREQ("user:password@localhost", data.authority);
    EXPECT_STREQ(nullptr, data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(23, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_scheme)
{
    std::string str = "http://";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ(nullptr, data.authority);
    EXPECT_STREQ(nullptr, data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_no_authority)
{
    std::string str = "http:///";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("", data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_query)
{
    std::string str = "http://?";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("", data.authority);
    EXPECT_STREQ(nullptr, data.path);
    EXPECT_STREQ("", data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_fragment)
{
    std::string str = "http://#";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("", data.authority);
    EXPECT_STREQ(nullptr, data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ("", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_scheme)
{
    std::string str = "%68ttp://user:password@localhost:80/?a=b#anchor";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_authority)
{
    std::string str = "http://us%65r:password@localhost:80/?a=b#anchor";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_authority_2)
{
    std::string str = "user:password@loc%61lhost:80";
    const uri::first_pass_data data = check_1st_pass(str, true, true);
    EXPECT_STREQ(nullptr, data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ(nullptr, data.path);
    EXPECT_STREQ(nullptr, data.query);
    EXPECT_STREQ(nullptr, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_path)
{
    std::string str = "http://user:password@localhost:80/%20?a=b#anchor";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ(" ", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_query)
{
    std::string str = "http://user:password@localhost:80/?%61=b#anchor";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_fragment)
{
    std::string str = "http://user:password@localhost:80/?a=b#%61nchor";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_at_the_end)
{
    std::string str = "http://user:password@localhost:80/?a=b#ancho%72";
    const uri::first_pass_data data = check_1st_pass(str, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, path_only)
{
    std::string str = "/";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ("", u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, http_localhost)
{
    std::string str = "http://localhost/";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("", u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, complete_uri)
{
    std::string str = "http://user:password@localhost:80/?a=b#anchor";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ("user:password", u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("", u->path());
    EXPECT_STREQ("a=b", u->query());
    EXPECT_STREQ("anchor", u->fragment());
}

TEST_F(uri_test, erroneous_port_1)
{
    std::string str = "http://localhost:80000/";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("", u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, erroneous_port_2)
{
    std::string str = "http://localhost:8x/";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("", u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, mailto_user_at_localhost)
{
    std::string str = "mailto://user@localhost";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::MAILTO, u->scheme());
    EXPECT_STREQ("user", u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, userinfo_at_localhost)
{
    std::string str = "user:pass@localhost/x";
    const std::unique_ptr<uri> u = check_parse(str, true, true);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ("user:pass", u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ("x", u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, no_authority)
{
    std::string str = "http:/";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("", u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, query_only)
{
    std::string str = "http:?a=b";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ("a=b", u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, fragment_only)
{
    std::string str = "http:#anchor";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ("anchor", u->fragment());
}

TEST_F(uri_test, no_scheme_no_authority)
{
    std::string str = "/";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ("", u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, http_localhost_with_percent_encoding)
{
    std::string str = "http://localhost/%48%65%6c%6C%6f%20%57%6F%72%6c%64%21";
    const std::unique_ptr<uri> u = check_parse(str);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("Hello World!", u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, http_localhost_with_erroneous_percent_encoding)
{
    std::string str = "http://localhost/%48%65%6g%6C%6f%20%57%6F%72%6c%64%21";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, http_localhost_with_erroneous_percent_encoding2)
{
    std::string str = "http://localhost/%48%65%6";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, erroneous_scheme)
{
    std::string str = "html:";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, only_scheme)
{
    std::string str = "http:";
    const std::unique_ptr<uri> u = check_parse(str, true);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, erroneous_query)
{
    std::string str = "/?a%2";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, erroneous_fragment)
{
    std::string str = "/#a%2";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, erroneous_authority1)
{
    std::string str = "http://loca%2lhost/";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

TEST_F(uri_test, erroneous_authority2)
{
    std::string str = "http://user@loca%2lhost/";
    const std::unique_ptr<uri> u = check_parse(str, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(nullptr, u->userinfo());
    EXPECT_STREQ(nullptr, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(nullptr, u->path());
    EXPECT_STREQ(nullptr, u->query());
    EXPECT_STREQ(nullptr, u->fragment());
}

} // namespace hutzn
