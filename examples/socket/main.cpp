/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include <rest.hpp>

#include <socket/connection.hpp>

void client()
{
    static const hutzn::buffer data = {0, 1, 2, 3};

    std::cout << "  connecting" << std::endl;
    auto c = hutzn::socket::connection::create("127.0.0.1", 30000);
    if (false == c->connect()) {
        std::cout << "  client not connected" << std::endl;
        abort();
    }

    std::cout << "  client receiving" << std::endl;
    hutzn::buffer data2;
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
    static const hutzn::buffer data = {0, 1, 2, 3};

    std::cout << "example_socket" << std::endl;

    std::cout << "  listening" << std::endl;
    hutzn::socket::listener_pointer listener =
        hutzn::socket::listen("127.0.0.1", 30000);
    std::thread thread(&client);

    std::cout << "  accepting" << std::endl;
    hutzn::socket::connection_pointer connection = listener->accept();

    std::cout << "  server sending" << std::endl;
    if (false == connection->send(data)) {
        std::cout << "  server aborts on send" << std::endl;
    }

    std::cout << "  server receiving" << std::endl;
    hutzn::buffer data2;
    if (false == connection->receive(data2, 8)) {
        std::cout << "  server aborts on receive" << std::endl;
    }

    std::cout << "  server comparing" << std::endl;
    if (data != data2) {
        std::cout << "  server aborts on comparing" << std::endl;
    }

    std::cout << "  joining" << std::endl;
    thread.join();

    std::cout << "  server terminating" << std::endl;
    return 0;
}
