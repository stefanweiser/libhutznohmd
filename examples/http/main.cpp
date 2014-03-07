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

#include <fstream>
#include <iostream>

#include <socket/socketinterface.hpp>
#include <http/request.hpp>

class Connection: public rest::socket::ConnectionSocketInterface
{
public:
    Connection(const std::string & filename)
        : m_stream(filename)
    {
        std::cout << " stream is_open() = " << m_stream.is_open() << "."
                  << std::endl;
    }

    virtual bool connect()
    {
        return false;
    }

    virtual void close() {}

    virtual bool receive(rest::Buffer & data, const size_t & maxSize)
    {
        size_t oldSize = data.size();
        data.resize(oldSize + maxSize);
        m_stream.read((char *) data.data() + oldSize, maxSize);
        size_t readBytes = m_stream.gcount();
        data.resize(oldSize + readBytes);
        std::cout << " read " << readBytes << " bytes." << std::endl;
        return (readBytes > 0);
    }

    virtual bool send(const rest::Buffer & /*data*/)
    {
        return true;
    }

    virtual bool send(const std::string & /*data*/)
    {
        return true;
    }

private:
    std::ifstream m_stream;
};

int main()
{
    std::cout << "example_http" << std::endl;

    std::shared_ptr<Connection> connection;
    connection = std::make_shared<Connection>("../examples/res/request0");
    rest::http::Request request(connection);
    request.parse();

    return 0;
}
