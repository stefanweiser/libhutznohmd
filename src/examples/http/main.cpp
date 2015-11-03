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

#include <fstream>
#include <iostream>

#include "http/request.hpp"
#include "hutzn.hpp"
#include "libhutznohmd/types.hpp"

class connection : public hutzn::connection_interface
{
public:
    explicit connection(const std::string& filename);

    void close(void) override;
    bool receive(hutzn::buffer& data, const size_t& max_size) override;
    bool send(const hutzn::buffer& data) override;
    bool send(const std::string& data) override;
    bool set_lingering_timeout(const int32_t& timeout) override;

private:
    std::ifstream stream_;
};

connection::connection(const std::string& filename)
    : stream_(filename)
{
    std::cout << " stream is_open() = " << stream_.is_open() << "."
              << std::endl;
}

void connection::close(void)
{
}

bool connection::receive(hutzn::buffer& data, const size_t& max_size)
{
    size_t read_bytes = 0;
    int32_t c = 0;
    while ((c >= 0) && (read_bytes < max_size)) {
        c = stream_.get();
        if (c >= 0) {
            data.push_back(static_cast<hutzn::char_t>(c));
            read_bytes++;
        }
    }
    std::cout << " read " << read_bytes << " bytes." << std::endl;
    return (read_bytes > 0);
}

bool connection::send(const hutzn::buffer& /*data*/)
{
    return true;
}

bool connection::send(const std::string& /*data*/)
{
    return true;
}

bool connection::set_lingering_timeout(const int32_t& /*timeout*/)
{
    return true;
}

int main(void)
{
    std::cout << "example_http" << std::endl;

    auto connection = std::make_shared<::connection>("./examples/res/request0");
    hutzn::http::request request(connection,
                                 hutzn::http::request::parameters{true});
    request.parse();

    return 0;
}
