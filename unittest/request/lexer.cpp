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

#include <request/lexer.hpp>

using namespace testing;

namespace hutzn
{

TEST(lexer, construction)
{
    connection_mock_pointer c = std::make_shared<connection_interface_mock>();
    EXPECT_CALL(*c, receive(_, _)).Times(1).WillOnce(Return(false));

    lexer l(c);
    EXPECT_EQ(nullptr, l.data(0));
    EXPECT_EQ(-1, l.get());
}

} // namespace hutzn
