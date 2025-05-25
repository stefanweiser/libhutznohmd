/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "request/uri.hpp"

using namespace testing;

namespace hutzn
{

class uri_test : public Test
{
public:
    static const size_t maximum_uri_enlargement = 3;

    uri::first_pass_data check_1st_pass(std::string& source,
                                        std::string& destination,
                                        const bool expect_sucess,
                                        const bool skip_scheme = false)
    {
        uri u;
        const char_t* src = &(source[0]);
        char_t* dest = &(destination[0]);
        size_t src_len = source.size();
        size_t dest_len = destination.size();
        uri::first_pass_data data;
        EXPECT_EQ(expect_sucess, u.parse_1st_pass(src, src_len, dest, dest_len,
                                                  data, skip_scheme));
        return data;
    }

    std::unique_ptr<uri> check_parse(const std::string& source,
                                     std::string& destination,
                                     const bool expect_sucess = true,
                                     const bool skip_scheme = false)
    {
        std::unique_ptr<uri> u = std::unique_ptr<uri>(new uri());
        const char_t* src = &(source[0]);
        char_t* dest = &(destination[0]);
        size_t src_len = source.size();
        size_t dest_len = destination.size();
        EXPECT_EQ(expect_sucess,
                  u->parse(src, src_len, dest, dest_len, skip_scheme));
        return u;
    }
};

TEST_F(uri_test, first_pass_empty)
{
    std::string source = "";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ(NULL, data.scheme);
    EXPECT_STREQ(NULL, data.authority);
    EXPECT_STREQ(NULL, data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_simple_path)
{
    std::string source = "/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ(NULL, data.scheme);
    EXPECT_STREQ(NULL, data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_localhost)
{
    std::string source = "http://localhost/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("localhost", data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(9, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_complete_uri)
{
    std::string source = "http://user:password@localhost:80/?a=b#anchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_authority)
{
    std::string source = "localhost";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ(NULL, data.scheme);
    EXPECT_STREQ("localhost", data.authority);
    EXPECT_STREQ(NULL, data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(9, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_authority_with_userinfo)
{
    std::string source = "user:password@localhost";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data =
        check_1st_pass(source, destination, true, true);
    EXPECT_STREQ(NULL, data.scheme);
    EXPECT_STREQ("user:password@localhost", data.authority);
    EXPECT_STREQ(NULL, data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(23, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_scheme)
{
    std::string source = "http://";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ(NULL, data.authority);
    EXPECT_STREQ(NULL, data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_no_authority)
{
    std::string source = "http:///";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ(NULL, data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_query)
{
    std::string source = "http://?";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ(NULL, data.authority);
    EXPECT_STREQ(NULL, data.path);
    EXPECT_STREQ("", data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_only_fragment)
{
    std::string source = "http://#";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ(NULL, data.authority);
    EXPECT_STREQ(NULL, data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ("", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(0, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_scheme)
{
    std::string source = "%68ttp://user:password@localhost:80/?a=b#anchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_authority)
{
    std::string source = "http://us%65r:password@localhost:80/?a=b#anchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_authority_2)
{
    std::string source = "user:password@loc%61lhost:80";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data =
        check_1st_pass(source, destination, true, true);
    EXPECT_STREQ(NULL, data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ(NULL, data.path);
    EXPECT_STREQ(NULL, data.query);
    EXPECT_STREQ(NULL, data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(0, data.path_size);
    EXPECT_EQ(0, data.query_size);
    EXPECT_EQ(0, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_path)
{
    std::string source = "http://user:password@localhost:80/%20?a=b#anchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("/ ", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(2, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_query)
{
    std::string source = "http://user:password@localhost:80/?%61=b#anchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_in_fragment)
{
    std::string source = "http://user:password@localhost:80/?a=b#%61nchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, first_pass_encoded_characters_at_the_end)
{
    std::string source = "http://user:password@localhost:80/?a=b#ancho%72";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data = check_1st_pass(source, destination, true);
    EXPECT_STREQ("http", data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(4, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, valid_minimal_uri)
{
    std::string source = "user:password@localhost:80/?a=b#anchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const uri::first_pass_data data =
        check_1st_pass(source, destination, true, true);
    EXPECT_STREQ(NULL, data.scheme);
    EXPECT_STREQ("user:password@localhost:80", data.authority);
    EXPECT_STREQ("/", data.path);
    EXPECT_STREQ("a=b", data.query);
    EXPECT_STREQ("anchor", data.fragment);
    EXPECT_EQ(0, data.scheme_size);
    EXPECT_EQ(26, data.authority_size);
    EXPECT_EQ(1, data.path_size);
    EXPECT_EQ(3, data.query_size);
    EXPECT_EQ(6, data.fragment_size);
}

TEST_F(uri_test, parse_nullpointer)
{
    uri u;
    EXPECT_FALSE(u.parse(NULL, 0, NULL, 0, false));
    EXPECT_EQ(uri_scheme::UNKNOWN, u.scheme());
    EXPECT_STREQ(NULL, u.userinfo());
    EXPECT_STREQ(NULL, u.host());
    EXPECT_EQ(0, u.port());
    EXPECT_STREQ(NULL, u.path());
    EXPECT_STREQ(NULL, u.query());
    EXPECT_STREQ(NULL, u.fragment());
}

TEST_F(uri_test, path_only)
{
    std::string source = "/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ("/", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, http_localhost)
{
    std::string source = "http://localhost/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("/", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, complete_uri)
{
    std::string source = "http://user:password@localhost:80/?a=b#anchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ("user:password", u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("/", u->path());
    EXPECT_STREQ("a=b", u->query());
    EXPECT_STREQ("anchor", u->fragment());
}

TEST_F(uri_test, erroneous_port_1)
{
    std::string source = "http://localhost:80000/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("/", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, erroneous_port_2)
{
    std::string source = "http://localhost:8x/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("/", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, localhost_with_path)
{
    std::string source = "localhost/x";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, true, true);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ("/x", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, userinfo_at_localhost)
{
    std::string source = "user:pass@localhost/x";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, true, true);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ("user:pass", u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ("/x", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, no_authority)
{
    std::string source = "http:/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("/", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, query_only)
{
    std::string source = "http:?a=b";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ("a=b", u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, fragment_only)
{
    std::string source = "http:#anchor";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ("anchor", u->fragment());
}

TEST_F(uri_test, no_scheme_no_authority)
{
    std::string source = "/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ("/", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, http_localhost_with_percent_encoding)
{
    std::string source =
        "http://localhost/%48%65%6c%6C%6f%20%57%6F%72%6c%64%21";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ("localhost", u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ("/Hello World!", u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, http_localhost_with_erroneous_percent_encoding)
{
    std::string source =
        "http://localhost/%48%65%6g%6C%6f%20%57%6F%72%6c%64%21";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, http_localhost_with_erroneous_percent_encoding2)
{
    std::string source = "http://localhost/%48%65%6";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, erroneous_scheme)
{
    std::string source = "html:";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, only_scheme)
{
    std::string source = "http:";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, true);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, erroneous_query)
{
    std::string source = "/?a%2";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, erroneous_fragment)
{
    std::string source = "/#a%2";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, erroneous_authority1)
{
    std::string source = "http://loca%2lhost/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, erroneous_authority2)
{
    std::string source = "http://user@loca%2lhost/";
    std::string destination(source.size() + maximum_uri_enlargement, ' ');
    const std::unique_ptr<uri> u = check_parse(source, destination, false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_STREQ(NULL, u->userinfo());
    EXPECT_STREQ(NULL, u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_STREQ(NULL, u->path());
    EXPECT_STREQ(NULL, u->query());
    EXPECT_STREQ(NULL, u->fragment());
}

TEST_F(uri_test, destination_buffer_to_small)
{
    uri u;
    char_t x[4] = "/";
    char_t y[4] = "   ";
    EXPECT_FALSE(u.parse(x, 1, y, 1 + maximum_uri_enlargement - 1));
    EXPECT_TRUE(u.parse(x, 1, y, 1 + maximum_uri_enlargement));
}

} // namespace hutzn
