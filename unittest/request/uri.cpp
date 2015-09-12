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

#include <libhutznohmd/mock_sockets.hpp>

#include <request/uri.hpp>

using namespace testing;

namespace hutzn
{

class uri_test : public ::testing::Test
{
public:
    void SetUp(void) override
    {
        connection_ = std::make_shared<connection_interface_mock>();
    }

    void TearDown(void) override
    {
        connection_.reset();
    }

    void setup_lexer(const std::string& chunk)
    {
        EXPECT_CALL(*connection_, receive(_, _))
            .Times(AtLeast(1))
            .WillOnce(Invoke([chunk](buffer& b, const size_t& m) {
                EXPECT_LE(chunk.size(), m);
                b.insert(b.begin(), chunk.begin(), chunk.end());
                return true;
            }))
            .WillRepeatedly(Return(false));
        lexer_ = std::unique_ptr<lexer>(new lexer(connection_));
        ASSERT_FALSE(lexer_->fetch_header());
    }

    std::unique_ptr<uri> parse(const bool expect_sucess = true)
    {
        std::unique_ptr<uri> u = std::unique_ptr<uri>(new uri());
        int32_t ch = lexer_->get();
        EXPECT_EQ(expect_sucess, u->parse(*lexer_, ch, false));
        return u;
    }

protected:
    connection_mock_pointer connection_;
    std::unique_ptr<lexer> lexer_;
};

TEST_F(uri_test, empty)
{
    setup_lexer("/");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, http_localhost)
{
    setup_lexer("http://localhost/");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, complete_uri)
{
    setup_lexer("http://user:password@localhost:80/?a=b#anchor");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string("user:password"), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string("a=b"), u->query());
    EXPECT_EQ(std::string("anchor"), u->fragment());
}

TEST_F(uri_test, erroneous_port)
{
    setup_lexer("http://localhost:80000/");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost:80000"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, mailto_user_at_localhost)
{
    setup_lexer("mailto://user@localhost");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::MAILTO, u->scheme());
    EXPECT_EQ(std::string("user"), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, no_authority)
{
    setup_lexer("http:/");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, query_only)
{
    setup_lexer("http:?a=b");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string("a=b"), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, fragment_only)
{
    setup_lexer("http:#anchor");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string("anchor"), u->fragment());
}

TEST_F(uri_test, no_scheme_no_authority)
{
    setup_lexer("/");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string("/"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, http_localhost_with_percent_encoding)
{
    setup_lexer("http://localhost/%48%65%6c%6C%6f%20%57%6F%72%6c%64%21");
    const std::unique_ptr<uri> u = parse();
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/Hello World!"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, http_localhost_with_erroneous_percent_encoding)
{
    setup_lexer("http://localhost/%48%65%6g%6C%6f%20%57%6F%72%6c%64%21");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/He"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, http_localhost_with_erroneous_percent_encoding2)
{
    setup_lexer("http://localhost/%48%65%6");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("localhost"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string("/He"), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, erroneous_scheme)
{
    setup_lexer("html");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, erroneous_scheme2)
{
    setup_lexer("http//");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, erroneous_scheme3)
{
    setup_lexer("http:");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, erroneous_query)
{
    setup_lexer("/?a%2");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(u->path(), std::string("/"));
    EXPECT_EQ(u->query(), std::string("a"));
    EXPECT_EQ(u->fragment(), std::string(""));
}

TEST_F(uri_test, erroneous_fragment)
{
    setup_lexer("/#a%2");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::UNKNOWN, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string(""), u->host());
    EXPECT_EQ(0, u->port());
    EXPECT_EQ(u->path(), std::string("/"));
    EXPECT_EQ(u->query(), std::string(""));
    EXPECT_EQ(u->fragment(), std::string("a"));
}

TEST_F(uri_test, erroneous_authority1)
{
    setup_lexer("http://loca%2lhost/");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string(""), u->userinfo());
    EXPECT_EQ(std::string("loca"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

TEST_F(uri_test, erroneous_authority2)
{
    setup_lexer("http://user@loca%2lhost/");
    const std::unique_ptr<uri> u = parse(false);
    EXPECT_EQ(uri_scheme::HTTP, u->scheme());
    EXPECT_EQ(std::string("user"), u->userinfo());
    EXPECT_EQ(std::string("loca"), u->host());
    EXPECT_EQ(80, u->port());
    EXPECT_EQ(std::string(""), u->path());
    EXPECT_EQ(std::string(""), u->query());
    EXPECT_EQ(std::string(""), u->fragment());
}

} // namespace hutzn
