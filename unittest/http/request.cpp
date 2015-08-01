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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <http/parser/utility/md5.hpp>
#include <http/request.hpp>

#include <libhutznohmd/mock_sockets.hpp>

using namespace testing;

namespace hutzn
{

namespace http
{

TEST(request, empty_body)
{
    auto socket = std::make_shared<connection_interface_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillOnce(
        Invoke([](buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-MD5: 1B2M2Y8AsgTpgAmY7PhCfg==\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = buffer(request_data.begin(), request_data.end());
            return true;
        }));
    EXPECT_TRUE(request.parse());

    std::array<uint8_t, 16> sum{{0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
                                 0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42,
                                 0x7E}};

    EXPECT_EQ(0, request.headers().size());
    EXPECT_EQ(sum, calculate_md5(request.data()));
    EXPECT_EQ(buffer(), request.data());
    EXPECT_EQ(media_type_interface::mime_type::CUSTOM,
              request.data_content_type().type());
    EXPECT_EQ(media_type_interface::mime_subtype::CUSTOM,
              request.data_content_type().subtype());
    EXPECT_EQ(std::string(), request.data_content_type().custom_type());
    EXPECT_EQ(std::string(), request.data_content_type().custom_subtype());
    EXPECT_EQ(method::GET, request.method());
    EXPECT_EQ(std::string("/"), request.request_uri().path());
    EXPECT_EQ(version::HTTP_1_1, request.version());
    EXPECT_EQ(true, request.keeps_connection());
}

TEST(request, wrong_md5)
{
    auto socket = std::make_shared<connection_interface_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillOnce(
        Invoke([](buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-MD5: 2B2M2Y8AsgTpgAmY7PhCfg==\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = buffer(request_data.begin(), request_data.end());
            return true;
        }));
    EXPECT_FALSE(request.parse());

    std::array<uint8_t, 16> sum{{0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
                                 0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42,
                                 0x7E}};

    EXPECT_EQ(0, request.headers().size());
    EXPECT_EQ(sum, calculate_md5(request.data()));
    EXPECT_EQ(buffer(), request.data());
    EXPECT_EQ(method::GET, request.method());
    EXPECT_EQ(std::string("/"), request.request_uri().path());
    EXPECT_EQ(version::HTTP_1_1, request.version());
    EXPECT_EQ(true, request.keeps_connection());
}

TEST(request, wrong_md5_but_no_check)
{
    auto socket = std::make_shared<connection_interface_mock>();
    request request(socket, request::parameters{false});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillOnce(
        Invoke([](buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-MD5: 2B2M2Y8AsgTpgAmY7PhCfg==\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = buffer(request_data.begin(), request_data.end());
            return true;
        }));
    EXPECT_TRUE(request.parse());

    std::array<uint8_t, 16> sum{{0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
                                 0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42,
                                 0x7E}};

    EXPECT_EQ(0, request.headers().size());
    EXPECT_EQ(sum, calculate_md5(request.data()));
    EXPECT_EQ(buffer(), request.data());
    EXPECT_EQ(method::GET, request.method());
    EXPECT_EQ(std::string("/"), request.request_uri().path());
    EXPECT_EQ(version::HTTP_1_1, request.version());
    EXPECT_EQ(true, request.keeps_connection());
}

TEST(request, parse)
{
    auto socket = std::make_shared<connection_interface_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillOnce(
        Invoke([](buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-Length: 1\r\n";
            stream << "Accept: text/html;q=1.0,\r\n";
            stream << " text/xml;q=0.5\r\n";
            stream << "Date: Wed, 01 Mar 2000 00:00:00 GMT\r\n";
            stream << "Connection: close\r\n";
            stream << "\r\n";
            stream << "00";
            std::string request_data = stream.str();
            data = buffer(request_data.begin(), request_data.end());
            return true;
        }));
    EXPECT_TRUE(request.parse());

    EXPECT_EQ(0, request.headers().size());
    EXPECT_EQ(buffer({'0'}), request.data());
    EXPECT_EQ(951868800, request.date());
    EXPECT_EQ(method::GET, request.method());
    EXPECT_EQ(std::string("/"), request.request_uri().path());
    EXPECT_EQ(version::HTTP_1_1, request.version());
    EXPECT_EQ(false, request.keeps_connection());
}

TEST(request, parse_false_return)
{
    auto socket = std::make_shared<connection_interface_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _))
        .Times(2)
        .WillOnce(Invoke([](buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-Length: 1\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = buffer(request_data.begin(), request_data.end());
            return true;
        }))
        .WillRepeatedly(Invoke([](buffer& /*data*/, const size_t & /*max_size*/)
                                   -> bool { return false; }));
    EXPECT_TRUE(request.parse());

    EXPECT_EQ(0, request.headers().size());
    EXPECT_EQ(true, request.data().empty());
    EXPECT_EQ(method::GET, request.method());
    EXPECT_EQ(std::string("/"), request.request_uri().path());
    EXPECT_EQ(version::HTTP_1_1, request.version());
}

TEST(request, parse_large_request)
{
    auto socket = std::make_shared<connection_interface_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _))
        .Times(3)
        .WillOnce(Invoke([](buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-Length: 2000\r\n";
            stream << "Accept: text/html,\r\n";
            stream << " text/xml\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = buffer(request_data.begin(), request_data.end());
            return true;
        }))
        .WillRepeatedly(
            Invoke([](buffer& data, const size_t & /*max_size*/) -> bool {
                buffer content(1000, '0');
                data.insert(data.end(), content.begin(), content.end());
                return true;
            }));
    EXPECT_TRUE(request.parse());

    EXPECT_EQ(0, request.headers().size());
    EXPECT_EQ(request.headers().end(), request.headers().find("abc"));
    EXPECT_EQ(2000, request.data().size());
    EXPECT_EQ(buffer(2000, '0'), request.data());
    time_t compare_time = time(NULL);
    EXPECT_GE(compare_time, request.date());
    EXPECT_LE(compare_time - 2, request.date());
    EXPECT_EQ(method::GET, request.method());
    EXPECT_EQ(std::string("/"), request.request_uri().path());
    EXPECT_EQ(version::HTTP_1_1, request.version());
}

TEST(request, no_needed_vailable)
{
    auto socket = std::make_shared<connection_interface_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillRepeatedly(Return(false));
    request.parse();
}

} // namespace http

} // namespace hutzn
