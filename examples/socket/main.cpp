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

#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include <socket/socketinterface.hpp>

rest::Buffer data = { 0, 1, 2, 3 };

void client()
{
    std::cout << "  connecting" << std::endl;
    rest::socket::ConnectionPtr c = rest::socket::connect("localhost", 30000);
    if (false == c->connect()) {
        std::cout << "  client not connected" << std::endl;
        abort();
    }

    std::cout << "  client receiving" << std::endl;
    rest::Buffer data2;
    if (false == c->receive(data2, 8)) {
        std::cout << "  client aborts on receive" << std::endl;
        abort();
    }

    std::cout << "  client comparing" << std::endl;
    if (data != data2) {
        std::cout << "  client aborts on compare" << std::endl;
    }

    std::cout << "  client sending" << std::endl;
    if (false == c->send(data2)) {
        std::cout << "  client aborts on send" << std::endl;
    }

    std::cout << "  client terminating" << std::endl;
}

int main()
{
    std::cout << "example_socket" << std::endl;

    std::cout << "  listening" << std::endl;
    rest::socket::ListenerPtr l = rest::socket::listen("localhost", 30000);
    std::thread t(&client);

    std::cout << "  accepting" << std::endl;
    rest::socket::ConnectionPtr c = l->accept();

    std::cout << "  server sending" << std::endl;
    if (false == c->send(data)) {
        std::cout << "  server aborts on send" << std::endl;
    }

    std::cout << "  server receiving" << std::endl;
    rest::Buffer data2;
    if (false == c->receive(data2, 8)) {
        std::cout << "  server aborts on receive" << std::endl;
    }

    std::cout << "  server comparing" << std::endl;
    if (data != data2) {
        std::cout << "  server aborts on comparing" << std::endl;
    }

    std::cout << "  joining" << std::endl;
    t.join();

    std::cout << "  server terminating" << std::endl;
    return 0;
}
