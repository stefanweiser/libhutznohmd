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

#include <libhutznohmd/request.hpp>
#include <request/mime_data.hpp>

using namespace testing;

namespace hutzn
{

namespace request
{

TEST(mime_data, initialization)
{
    mime_data<mime_type, uint8_t> t;
    EXPECT_EQ(t.parse_type("abc"), mime_type::INVALID);
    EXPECT_EQ(t.parse_type("ABC"), mime_type::INVALID);
}

} // namespace request

} // namespace hutzn
