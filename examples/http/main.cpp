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

#include <fstream>
#include <iostream>

#include <socket/socketinterface.hpp>
#include <http/request.hpp>

class connection: public rest::socket::connection_socket_interface
{
public:
    explicit connection(const std::string & filename);

    virtual bool connect();
    virtual void close();
    virtual bool receive(rest::buffer & data, const size_t & max_size);
    virtual bool send(const rest::buffer & data);
    virtual bool send(const std::string & data);

private:
    std::ifstream stream_;
};

connection::connection(const std::string & filename)
    : stream_(filename)
{
    std::cout << " stream is_open() = " << stream_.is_open() << "." << std::endl;
}

bool connection::connect()
{
    return false;
}

void connection::close()
{}

bool connection::receive(rest::buffer & data, const size_t & max_size)
{
    size_t old_size = data.size();
    data.resize(old_size + max_size);
    stream_.read((char *) data.data() + old_size, max_size);
    size_t read_bytes = stream_.gcount();
    data.resize(old_size + read_bytes);
    std::cout << " read " << read_bytes << " bytes." << std::endl;
    return (read_bytes > 0);
}

bool connection::send(const rest::buffer & /*data*/)
{
    return true;
}

bool connection::send(const std::string & /*data*/)
{
    return true;
}

int main()
{
    std::cout << "example_http" << std::endl;

    auto connection = std::make_shared<::connection>("../examples/res/request0");
    rest::http::request request(connection, rest::http::request::parameters {true});
    request.parse();

    return 0;
}
