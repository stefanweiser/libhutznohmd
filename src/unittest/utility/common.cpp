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

#include "utility/common.hpp"

using namespace testing;

namespace hutzn
{

TEST(common, check_range_upper_bound)
{
    auto fn = [](const size_t v) { return check_range<size_t, 2, 3>(v); };

    EXPECT_TRUE(fn(2));
    EXPECT_TRUE(fn(3));
    EXPECT_FALSE(fn(4));
}

TEST(common, check_range_lower_bound)
{
    auto fn = [](const size_t v) { return check_range<size_t, 2, 3>(v); };

    EXPECT_FALSE(fn(1));
    EXPECT_TRUE(fn(2));
    EXPECT_TRUE(fn(3));
}

TEST(common, is_valid_url_path_valid)
{
    EXPECT_TRUE(is_valid_uri_path("/foo/bar"));
    EXPECT_TRUE(is_valid_uri_path("/foo/bar/"));
    EXPECT_TRUE(is_valid_uri_path("/"));
}

TEST(common, is_valid_url_path_invalid)
{
    EXPECT_FALSE(is_valid_uri_path("//"));
    EXPECT_FALSE(is_valid_uri_path("/foo//bar"));
    EXPECT_FALSE(is_valid_uri_path("/foo/bar//"));
    EXPECT_FALSE(is_valid_uri_path("foo"));
    EXPECT_FALSE(is_valid_uri_path("foo/bar"));
    EXPECT_FALSE(is_valid_uri_path("/#"));
    EXPECT_FALSE(is_valid_uri_path("/?"));
}

} // namespace hutzn
