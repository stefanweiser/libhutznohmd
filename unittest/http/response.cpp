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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/response.hpp>

#include <socket/mocksocketinterface.hpp>

using namespace testing;

TEST(Response, ConstructionDestruction)
{
    std::shared_ptr<rest::http::Response> response;
    response = std::make_shared<rest::http::Response>(rest::socket::ConnectionPtr());
    EXPECT_EQ(response->m_connection, rest::socket::ConnectionPtr());
    EXPECT_EQ(response->m_statusCode, rest::http::StatusCode::InternalServerError);
    EXPECT_EQ(response->m_version, rest::http::Version::HTTP_1_1);
    EXPECT_EQ(response->m_headers.size(), 0);
    EXPECT_EQ(response->m_data.size(), 0);
}

TEST(Response, SetAndDeliver)
{
    std::shared_ptr<rest::socket::MockConnectionSocket> socket;
    socket = std::make_shared<rest::socket::MockConnectionSocket>();
    rest::http::Response response(socket);

    response.setData({ '0' });
    response.setHeader("Content-Length", "1");
    response.setStatusCode(rest::http::StatusCode::Ok);
    response.setVersion(rest::http::Version::HTTP_1_1);

    EXPECT_CALL(*socket, send(_))
    .Times(2)
    .WillRepeatedly(Return(true));
    EXPECT_NO_THROW(response.deliver());

    response.setVersion(rest::http::Version::HTTP_1_0);
    EXPECT_CALL(*socket, send(_))
    .Times(2)
    .WillRepeatedly(Return(true));
    EXPECT_NO_THROW(response.deliver());

    response.setVersion(rest::http::Version::HTTP_UNKNOWN);
    EXPECT_CALL(*socket, send(_))
    .Times(0);
    EXPECT_THROW(response.deliver(), std::exception);
}
