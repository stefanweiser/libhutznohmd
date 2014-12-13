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

#ifndef LIBHUTZNOHMD_HTTP_REQUEST_HPP
#define LIBHUTZNOHMD_HTTP_REQUEST_HPP

#include <ctime>
#include <map>
#include <memory>

#include <socket/listener.hpp>

#include <rest.hpp>

#include <http/parser/request_parser.hpp>

namespace hutzn
{

namespace http
{

class request : public request_interface
{
public:
    struct parameters
    {
        bool check_md5;
    };

    explicit request(const hutzn::socket::connection_pointer& connection,
                     const parameters& param);
    bool parse();

    virtual hutzn::http::method method() const;
    virtual const uri_interface& request_uri() const;
    virtual hutzn::http::version version() const;
    virtual const std::map<std::string, std::string>& headers() const;
    virtual const hutzn::buffer& data() const;
    virtual const media_type_interface& data_content_type() const;
    virtual time_t date() const;
    virtual bool keeps_connection() const;

    int32_t get();
    int32_t peek();

private:
    hutzn::socket::connection_pointer connection_;
    hutzn::buffer buffer_;
    hutzn::http::request_parser request_parser_;
    hutzn::buffer data_;
    size_t index_;
    parameters parameters_;
};

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HTTP_REQUEST_HPP
