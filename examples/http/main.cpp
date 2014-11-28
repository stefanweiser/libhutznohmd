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

#include <fstream>
#include <iostream>

#include <rest.hpp>
#include <http/request.hpp>

class connection : public rest::socket::connection_interface
{
public:
    explicit connection(const std::string& filename);

    virtual bool connect();
    virtual void close();
    virtual bool receive(rest::buffer& data, const size_t& max_size);
    virtual bool send(const rest::buffer& data);
    virtual bool send(const std::string& data);
    virtual bool set_lingering_timeout(const int& timeout);
    virtual int socket() const;

private:
    std::ifstream stream_;
};

connection::connection(const std::string& filename)
    : stream_(filename)
{
    std::cout << " stream is_open() = " << stream_.is_open() << "."
              << std::endl;
}

bool connection::connect()
{
    return false;
}

void connection::close()
{
}

bool connection::receive(rest::buffer& data, const size_t& max_size)
{
    size_t read_bytes = 0;
    int c = 0;
    while ((c >= 0) && (read_bytes < max_size)) {
        c = stream_.get();
        if (c >= 0) {
            data.push_back(static_cast<uint8_t>(c));
            read_bytes++;
        }
    }
    std::cout << " read " << read_bytes << " bytes." << std::endl;
    return (read_bytes > 0);
}

bool connection::send(const rest::buffer& /*data*/)
{
    return true;
}

bool connection::send(const std::string& /*data*/)
{
    return true;
}

bool connection::set_lingering_timeout(const int& /*timeout*/)
{
    return true;
}

int connection::socket() const
{
    return 0;
}

int main()
{
    std::cout << "example_http" << std::endl;

    auto connection = std::make_shared<::connection>("./examples/res/request0");
    rest::http::request request(connection,
                                rest::http::request::parameters{true});
    request.parse();

    return 0;
}
