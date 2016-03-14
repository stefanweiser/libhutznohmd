/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "libhutznohmd/types.hpp"
#include "request/timestamp.hpp"
#include "request/uri.hpp"

void test_date_parser(const std::string& date_string)
{
    // This initializes all static variables, that else would sophisticate
    // the results.
    hutzn::parse_timestamp(date_string.c_str(), date_string.size());

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 1000000; i++) {
        hutzn::parse_timestamp(date_string.c_str(), date_string.size());
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto diff =
        std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << std::setprecision(0) << (diff.count() * 1000.0)
              << " ns for date string: " << date_string << std::endl;
}

void test_uri_parser(std::string uri)
{
    {
        // This initializes all static variables, that else would sophisticate
        // the results.
        hutzn::uri u;
        std::string tmp = uri;
        size_t src_len = tmp.size();
        std::string tmp2(src_len + 4, ' ');
        size_t dest_len = tmp2.size();
        hutzn::char_t* src = &(tmp[0]);
        hutzn::char_t* dest = &(tmp2[0]);
        u.parse(src, src_len, dest, dest_len, false);
    }

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 1000000; i++) {
        hutzn::uri u;
        std::string tmp = uri;
        std::string tmp2(tmp.size() + 1, ' ');
        size_t src_len = tmp.size();
        size_t dest_len = tmp2.size();
        hutzn::char_t* src = &(tmp[0]);
        hutzn::char_t* dest = &(tmp2[0]);
        u.parse(src, src_len, dest, dest_len, false);
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto diff =
        std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << std::setprecision(3) << (diff.count() * 1000.0)
              << " ns for uri: " << uri << std::endl;
}

int main(void)
{
    std::cout << "example_performance" << std::endl;

    test_date_parser("Sun, 06 Nov 1994 08:49:37 GMT");
    test_date_parser("Sunday, 06-Nov-94 08:49:37 GMT");
    test_date_parser("Sun Nov  6 08:49:37 1994");

    test_uri_parser("/");
    test_uri_parser("http://localhost/");
    test_uri_parser("http://localhost:80/");
    test_uri_parser("http://user:pw@localhost:80/");
    test_uri_parser("http://user:pw@localhost:80/?a=b#anchor");

    return 0;
}
