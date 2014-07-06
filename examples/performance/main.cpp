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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <http/parser/requestparser.hpp>
#include <http/parser/utility/date.hpp>
#include <http/parser/utility/trie.hpp>
#include <http/parser/utility/uri.hpp>

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

} // namespace

void test_trie_parse(const std::string & token)
{
    const std::vector<trie<header_type>::value_info> values = {{
            trie<header_type>::value_info{"content-length", header_type::CONTENT_LENGTH},
            trie<header_type>::value_info{"content-type", header_type::CONTENT_TYPE},
            trie<header_type>::value_info{"date", header_type::DATE}
        }
    };
    trie<header_type> t(values, header_type::CUSTOM);
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 1000000; i++) {
        string_index_pair p(token, 0);
        lexer l(anonymous_int_function(&get_char, &p),
                anonymous_int_function(&peek_char, &p));
        int32_t character = l.get();
        push_back_string<4> fail_safe_result;
        t.parse(character, fail_safe_result, l);
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << std::setprecision(3) << (diff.count() * 1000.0)
              << " ns for token: <" << token << ">" << std::endl;
}

void test_req_parser(const std::string & request)
{
    {
        // This initializes all static variables, that else would sophisticate the results.
        string_index_pair p(request, 0);
        request_parser parser(anonymous_int_function(&get_char, &p),
                              anonymous_int_function(&peek_char, &p));
        parser.parse();
    }
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 1000; i++) {
        string_index_pair p(request, 0);
        request_parser parser(anonymous_int_function(&get_char, &p),
                              anonymous_int_function(&peek_char, &p));
        parser.parse();
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << std::setprecision(3) << (diff.count() * 1000.0)
              << " us for request: \n" << request << "---\n\n" << std::endl;
}

void test_http_date_parser(const std::string & date_string)
{
    {
        // This initializes all static variables, that else would sophisticate the results.
        string_index_pair p(date_string, 0);
        lexer l(anonymous_int_function(&get_char, &p),
                anonymous_int_function(&peek_char, &p));
        int32_t result = l.get();
        parse_timestamp(result, l);
    }

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 1000; i++) {
        string_index_pair p(date_string, 0);
        lexer l(anonymous_int_function(&get_char, &p),
                anonymous_int_function(&peek_char, &p));
        int32_t result = l.get();
        parse_timestamp(result, l);
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << std::setprecision(3) << (diff.count() * 1000.0)
              << " us for date string: " << date_string << std::endl;
}

void test_uri_parser(const std::string & uri)
{
    {
        // This initializes all static variables, that else would sophisticate the results.
        string_index_pair p(uri, 0);
        lexer l(anonymous_int_function(&get_char, &p),
                anonymous_int_function(&peek_char, &p));
        int32_t result = l.get();
        rest::http::uri u;
        u.parse(l, result);
    }

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 1000000; i++) {
        string_index_pair p(uri, 0);
        lexer l(anonymous_int_function(&get_char, &p),
                anonymous_int_function(&peek_char, &p));
        int32_t result = l.get();
        rest::http::uri u;
        u.parse(l, result);
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << std::setprecision(3) << (diff.count() * 1000.0)
              << " ns for uri: " << uri << std::endl;
}

} // namespace http

} // namespace rest


int main()
{
    std::cout << "example_performance" << std::endl;

    rest::http::test_req_parser("GET / HTTP/1.1\r\n\r\n");
    rest::http::test_req_parser("GET /what/a/long/url HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    rest::http::test_req_parser("GET /what/a/long/url HTTP/1.1\r\nContent-Length: 0\r\n"
                                "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                                "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                                "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                                "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                                "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                                "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                                "Content-Length: 0\r\n\r\n");

    rest::http::test_http_date_parser("Sun, 06 Nov 1994 08:49:37 GMT");
    rest::http::test_http_date_parser("Sunday, 06-Nov-94 08:49:37 GMT");
    rest::http::test_http_date_parser("Sun Nov  6 08:49:37 1994");

    rest::http::test_trie_parse("");
    rest::http::test_trie_parse("content-length");
    rest::http::test_trie_parse("content-type");
    rest::http::test_trie_parse("date");

    rest::http::test_uri_parser("/");
    rest::http::test_uri_parser("http://localhost/");
    rest::http::test_uri_parser("http://localhost:80/");
    rest::http::test_uri_parser("http://user:pw@localhost:80/");
    rest::http::test_uri_parser("http://user:pw@localhost:80/?a=b#anchor");

    return 0;
}
