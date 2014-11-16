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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <http/parser/utility/md5.hpp>
#include <http/request.hpp>

#include <mock_socket_interfaces.hpp>

using namespace testing;

namespace rest
{

namespace http
{

TEST(request, empty_body)
{
    auto socket = std::make_shared<rest::socket::connection_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillOnce(
        Invoke([](rest::buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-MD5: 1B2M2Y8AsgTpgAmY7PhCfg==\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = rest::buffer(request_data.begin(), request_data.end());
            return true;
        }));
    EXPECT_TRUE(request.parse());

    std::array<uint8_t, 16> sum{{0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
                                 0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42,
                                 0x7E}};

    EXPECT_EQ(request.headers().size(), 0);
    EXPECT_EQ(calculate_md5(request.data()), sum);
    EXPECT_EQ(request.data(), rest::buffer());
    EXPECT_EQ(request.data_content_type().type(),
              media_type_interface::mime_type::CUSTOM);
    EXPECT_EQ(request.data_content_type().subtype(),
              media_type_interface::mime_subtype::CUSTOM);
    EXPECT_EQ(request.data_content_type().custom_type(), std::string());
    EXPECT_EQ(request.data_content_type().custom_subtype(), std::string());
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.request_uri().path(), std::string("/"));
    EXPECT_EQ(request.version(), version::HTTP_1_1);
    EXPECT_EQ(request.keeps_connection(), true);
}

TEST(request, wrong_md5)
{
    auto socket = std::make_shared<rest::socket::connection_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillOnce(
        Invoke([](rest::buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-MD5: 2B2M2Y8AsgTpgAmY7PhCfg==\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = rest::buffer(request_data.begin(), request_data.end());
            return true;
        }));
    EXPECT_FALSE(request.parse());

    std::array<uint8_t, 16> sum{{0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
                                 0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42,
                                 0x7E}};

    EXPECT_EQ(request.headers().size(), 0);
    EXPECT_EQ(calculate_md5(request.data()), sum);
    EXPECT_EQ(request.data(), rest::buffer());
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.request_uri().path(), std::string("/"));
    EXPECT_EQ(request.version(), version::HTTP_1_1);
    EXPECT_EQ(request.keeps_connection(), true);
}

TEST(request, wrong_md5_but_no_check)
{
    auto socket = std::make_shared<rest::socket::connection_mock>();
    request request(socket, request::parameters{false});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillOnce(
        Invoke([](rest::buffer& data, const size_t & /*max_size*/) -> bool {
            std::stringstream stream;
            stream << "GET / HTTP/1.1\r\n";
            stream << "Content-MD5: 2B2M2Y8AsgTpgAmY7PhCfg==\r\n";
            stream << "\r\n";
            std::string request_data = stream.str();
            data = rest::buffer(request_data.begin(), request_data.end());
            return true;
        }));
    EXPECT_TRUE(request.parse());

    std::array<uint8_t, 16> sum{{0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
                                 0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42,
                                 0x7E}};

    EXPECT_EQ(request.headers().size(), 0);
    EXPECT_EQ(calculate_md5(request.data()), sum);
    EXPECT_EQ(request.data(), rest::buffer());
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.request_uri().path(), std::string("/"));
    EXPECT_EQ(request.version(), version::HTTP_1_1);
    EXPECT_EQ(request.keeps_connection(), true);
}

TEST(request, parse)
{
    auto socket = std::make_shared<rest::socket::connection_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillOnce(
        Invoke([](rest::buffer& data, const size_t & /*max_size*/) -> bool {
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
            data = rest::buffer(request_data.begin(), request_data.end());
            return true;
        }));
    EXPECT_TRUE(request.parse());

    EXPECT_EQ(request.headers().size(), 0);
    EXPECT_EQ(request.data(), rest::buffer({'0'}));
    EXPECT_EQ(request.date(), 951868800);
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.request_uri().path(), std::string("/"));
    EXPECT_EQ(request.version(), version::HTTP_1_1);
    EXPECT_EQ(request.keeps_connection(), false);
}

TEST(request, parse_false_return)
{
    auto socket = std::make_shared<rest::socket::connection_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _))
        .Times(2)
        .WillOnce(Invoke(
            [](rest::buffer& data, const size_t & /*max_size*/) -> bool {
                std::stringstream stream;
                stream << "GET / HTTP/1.1\r\n";
                stream << "Content-Length: 1\r\n";
                stream << "\r\n";
                std::string request_data = stream.str();
                data = rest::buffer(request_data.begin(), request_data.end());
                return true;
            }))
        .WillRepeatedly(
            Invoke([](rest::buffer& /*data*/, const size_t & /*max_size*/)
                       -> bool { return false; }));
    EXPECT_TRUE(request.parse());

    EXPECT_EQ(request.headers().size(), 0);
    EXPECT_EQ(request.data().empty(), true);
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.request_uri().path(), std::string("/"));
    EXPECT_EQ(request.version(), version::HTTP_1_1);
}

TEST(request, parse_large_request)
{
    auto socket = std::make_shared<rest::socket::connection_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _))
        .Times(3)
        .WillOnce(Invoke(
            [](rest::buffer& data, const size_t & /*max_size*/) -> bool {
                std::stringstream stream;
                stream << "GET / HTTP/1.1\r\n";
                stream << "Content-Length: 2000\r\n";
                stream << "Accept: text/html,\r\n";
                stream << " text/xml\r\n";
                stream << "\r\n";
                std::string request_data = stream.str();
                data = rest::buffer(request_data.begin(), request_data.end());
                return true;
            }))
        .WillRepeatedly(
            Invoke([](rest::buffer& data, const size_t & /*max_size*/) -> bool {
                rest::buffer content(1000, '0');
                data.insert(data.end(), content.begin(), content.end());
                return true;
            }));
    EXPECT_TRUE(request.parse());

    EXPECT_EQ(request.headers().size(), 0);
    EXPECT_EQ(request.headers().find("abc"), request.headers().end());
    EXPECT_EQ(request.data().size(), 2000);
    EXPECT_EQ(request.data(), rest::buffer(2000, '0'));
    time_t compare_time = time(NULL);
    EXPECT_LE(request.date(), compare_time);
    EXPECT_GE(request.date(), compare_time - 2);
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.request_uri().path(), std::string("/"));
    EXPECT_EQ(request.version(), version::HTTP_1_1);
}

TEST(request, no_needed_vailable)
{
    auto socket = std::make_shared<rest::socket::connection_mock>();
    request request(socket, request::parameters{true});

    EXPECT_CALL(*socket, receive(_, _)).Times(1).WillRepeatedly(Return(false));
    request.parse();
}

} // namespace http

} // namespace rest
