/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

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
#include <iostream>
#include <sstream>

#include <http/parser/lexer.h>

int main()
{
    std::cout << "example_performance" << std::endl;

    std::stringstream str("GET / HTTP/1.1\r\n\r\n");
    rest::http::HttpParser parser(std::bind((int(std::istream::*)()) &std::istream::get, &str),
                                  std::bind(&std::istream::peek, &str));

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    parser.parse();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << std::fixed << (diff.count() * 1000.0) << " ms." << std::endl;

    return 0;
}
