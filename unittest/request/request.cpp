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

#include <libhutznohmd/mock_sockets.hpp>

#include <request/request.hpp>

using namespace testing;

namespace hutzn
{

class request_test : public ::testing::Test
{
public:
    void SetUp(void) override
    {
        connection_ = std::make_shared<connection_interface_mock>();
    }

    void TearDown(void) override
    {
        connection_.reset();
    }

protected:
    connection_mock_pointer connection_;
};

TEST_F(request_test, construction)
{
    request r{connection_};
    EXPECT_EQ(http_verb::GET, r.method());
    EXPECT_EQ(nullptr, r.path());
    EXPECT_EQ(nullptr, r.host());
    EXPECT_EQ(nullptr, r.query(nullptr));
    EXPECT_EQ(nullptr, r.fragment());
    EXPECT_EQ(http_version::HTTP_UNKNOWN, r.version());
    EXPECT_EQ(nullptr, r.header_value(nullptr));
    EXPECT_EQ(false, r.keeps_connection());
    EXPECT_EQ(0, r.date());
    EXPECT_EQ(nullptr, r.content());
    EXPECT_EQ(0, r.content_length());
    EXPECT_EQ(mime(mime_type::INVALID, mime_subtype::INVALID),
              r.content_type());

    void* handle = nullptr;
    mime m{mime_type::INVALID, mime_subtype::INVALID};
    EXPECT_EQ(false, r.accept(handle, m));

    EXPECT_EQ(http_expectation::UNKNOWN, r.expect());
    EXPECT_EQ(nullptr, r.from());
    EXPECT_EQ(nullptr, r.referer());
    EXPECT_EQ(nullptr, r.user_agent());
}

} // namespace hutzn
