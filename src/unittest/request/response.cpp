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

#include "libhutznohmd/mock_sockets.hpp"
#include "request/response.hpp"

using namespace testing;

namespace hutzn
{

class response_test : public Test
{
public:
    void SetUp(void) override
    {
        connection_ = std::make_shared<connection_mock>();
    }

    void TearDown(void) override
    {
        connection_.reset();
    }

protected:
    connection_mock_ptr connection_;
};

TEST_F(response_test, construction)
{
    response r{connection_};
    r.set_content(buffer(), false);
    r.set_content_location(NULL);
    EXPECT_FALSE(r.set_header(NULL, NULL));
    r.set_location(NULL);
    EXPECT_FALSE(r.set_retry_after(0));
    r.set_server(NULL);
}

} // namespace hutzn
