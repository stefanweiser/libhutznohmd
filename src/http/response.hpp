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

#ifndef LIBHUTZNOHMD_HTTP_RESPONSE_HPP
#define LIBHUTZNOHMD_HTTP_RESPONSE_HPP

#include <map>
#include <memory>

#include <socket/listener.hpp>

#include <hutzn.hpp>

namespace hutzn
{

namespace http
{

class response : public response_interface
{
public:
    explicit response(const hutzn::socket::connection_pointer& connection);
    void deliver();

    void set_status_code(
        const hutzn::request::http_status_code& status_code) override;
    void set_version(const hutzn::http::version& version) override;
    void set_header(const std::string& key, const std::string& value) override;
    void set_data(const hutzn::buffer& data) override;

private:
    static void deliver_version(std::ostream& os, const version& version);
    static void deliver_status_code_text(
        std::ostream& os, const hutzn::request::http_status_code& code);

    hutzn::socket::connection_pointer connection_;

    hutzn::request::http_status_code status_code_;
    version version_;
    std::map<std::string, std::string> headers_;
    hutzn::buffer data_;
};

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HTTP_RESPONSE_HPP
