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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_REQUEST_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_REQUEST_HPP

#include <gmock/gmock.h>

#include <hutzn.hpp>

namespace hutzn
{

namespace request
{

class request_interface_mock : public request_interface
{
public:
    MOCK_CONST_METHOD0(method, http_verb());
    MOCK_CONST_METHOD0(path, const char*());
    MOCK_CONST_METHOD0(host, const char*());
    MOCK_CONST_METHOD1(query, const char*(const char* const));
    MOCK_CONST_METHOD0(fragment, const char*());
    MOCK_CONST_METHOD0(version, http_version());
    MOCK_CONST_METHOD1(header_value, const char*(const char* const));
    MOCK_CONST_METHOD0(keeps_connection, bool());
    MOCK_CONST_METHOD0(date, time_t());
    MOCK_CONST_METHOD0(content, void*());
    MOCK_CONST_METHOD0(content_length, size_t());
    MOCK_CONST_METHOD0(content_type, hutzn::request::mime());
    MOCK_CONST_METHOD2(accept, bool(void*&, hutzn::request::mime&));
    MOCK_CONST_METHOD0(expect, http_expectation());
    MOCK_CONST_METHOD0(from, const char*());
    MOCK_CONST_METHOD0(referer, const char*());
    MOCK_CONST_METHOD0(user_agent, const char*());
};

using request_mock_pointer = std::shared_ptr<request_interface_mock>;

class response_interface_mock : public response_interface
{
public:
    MOCK_METHOD2(set_header, bool(const char* const, const char* const));
    MOCK_METHOD2(set_content, void(const hutzn::buffer&, const bool));
    MOCK_METHOD1(set_content_location, void(const char* const));
    MOCK_METHOD1(set_location, void(const char* const));
    MOCK_METHOD1(set_retry_after, bool(const time_t));
    MOCK_METHOD1(set_server, void(const char* const));
};

using response_mock_pointer = std::shared_ptr<response_interface_mock>;

} // namespace request

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_REQUEST_HPP
