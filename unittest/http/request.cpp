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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/request.hpp>

#include <socket/mocksocketinterface.hpp>

using namespace testing;

namespace rest
{

namespace http
{

TEST(request, construction_destruction)
{
    auto r = std::make_shared<request>(rest::socket::connection_pointer());
    EXPECT_EQ(r->connection_, rest::socket::connection_pointer());
    EXPECT_EQ(r->buffer_.size(), 0);
    EXPECT_EQ(r->data_.size(), 0);
    EXPECT_EQ(r->index_, 0);
}

TEST(request, parse)
{
    auto socket = std::make_shared<rest::socket::connection_socket_mock>();
    request request(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(1)
    .WillOnce(Invoke([](rest::buffer & data, const size_t & /*max_size*/) -> bool {
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
    request.parse();

    EXPECT_EQ(request.http_parser_.content_length(), 1);
    EXPECT_EQ(request.http_parser_.httpscan_.headers_.size(), 1);
    EXPECT_EQ(request.data(), rest::buffer({ '0' }));
    EXPECT_EQ(request.date(), 951868800);
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.url(), "/");
    EXPECT_EQ(request.version(), version::HTTP_1_1);
    EXPECT_EQ(request.is_keep_connection(), false);
}

TEST(request, parse_false_return)
{
    auto socket = std::make_shared<rest::socket::connection_socket_mock>();
    request request(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(2)
    .WillOnce(Invoke([](rest::buffer & data, const size_t & /*max_size*/) -> bool {
        std::stringstream stream;
        stream << "GET / HTTP/1.1\r\n";
        stream << "Content-Length: 1\r\n";
        stream << "\r\n";
        std::string request_data = stream.str();
        data = rest::buffer(request_data.begin(), request_data.end());
        return true;
    }))
    .WillRepeatedly(Invoke([](rest::buffer & /*data*/, const size_t & /*max_size*/) -> bool {
        return false;
    }));
    request.parse();

    EXPECT_EQ(request.http_parser_.content_length(), 1);
    EXPECT_EQ(request.http_parser_.httpscan_.headers_.empty(), true);
    EXPECT_EQ(request.data().empty(), true);
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.url(), "/");
    EXPECT_EQ(request.version(), version::HTTP_1_1);
}

TEST(request, parse_large_request)
{
    auto socket = std::make_shared<rest::socket::connection_socket_mock>();
    request request(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(3)
    .WillOnce(Invoke([](rest::buffer & data, const size_t & /*max_size*/) -> bool {
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
    .WillRepeatedly(Invoke([](rest::buffer & data, const size_t & /*max_size*/) -> bool {
        rest::buffer content(1000, '0');
        data.insert(data.end(), content.begin(), content.end());
        return true;
    }));
    request.parse();

    EXPECT_EQ(request.http_parser_.content_length(), 2000);
    EXPECT_EQ(request.http_parser_.httpscan_.headers_.size(), 1);
    EXPECT_EQ(request.header("abc"), "");
    EXPECT_EQ(request.data().size(), 2000);
    EXPECT_EQ(request.data(), rest::buffer(2000, '0'));
    time_t compare_time = time(NULL);
    EXPECT_LE(request.date(), compare_time);
    EXPECT_GE(request.date(), compare_time - 2);
    EXPECT_EQ(request.method(), method::GET);
    EXPECT_EQ(request.url(), "/");
    EXPECT_EQ(request.version(), version::HTTP_1_1);
}

TEST(request, no_needed_httpscan_available)
{
    auto socket = std::make_shared<rest::socket::connection_socket_mock>();
    request request(socket);

    EXPECT_CALL(*socket, receive(_, _))
    .Times(1)
    .WillRepeatedly(Return(false));
    request.parse();
}

} // namespace http

} // namespace rest
