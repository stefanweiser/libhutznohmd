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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <http/parser/httpparser.hpp>
#include <http/parser/header/httpdateparser.h>

int anonymous_get(void * handle)
{
    return static_cast<std::istream *>(handle)->get();
}

int anonymous_peek(void * handle)
{
    return static_cast<std::istream *>(handle)->peek();
}

void test_http_parser(const std::string & request)
{
    std::stringstream s(request);
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 1000000; i++) {
        rest::http::http_parser parser(anonymous_int_function(&anonymous_get, &s),
                                       anonymous_int_function(&anonymous_peek, &s));
        parser.parse();
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << std::setprecision(3) << (diff.count() * 1000.0)
              << " ns for request: \n" << request << "---\n\n" << std::endl;
}

void test_http_date_parser(const std::string & date_string)
{
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 1000; i++) {
        rest::http::http_date_parser parser(date_string);
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << std::setprecision(3) << (diff.count() * 1000.0)
              << " us for date string: " << date_string << std::endl;
}

int main()
{
    std::cout << "example_performance" << std::endl;

    test_http_parser("GET / HTTP/1.1\r\n\r\n");
    test_http_parser("GET /what/a/long/url HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    test_http_parser("GET /what/a/long/url HTTP/1.1\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                     "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                     "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                     "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                     "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                     "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n"
                     "Content-Length: 0\r\nContent-Length: 0\r\nContent-Length: 0\r\n\r\n");

    test_http_date_parser("Sun, 06 Nov 1994 08:49:37 GMT");
    test_http_date_parser("Sunday, 06-Nov-94 08:49:37 GMT");
    test_http_date_parser("Sun Nov  6 08:49:37 1994");

    return 0;
}
