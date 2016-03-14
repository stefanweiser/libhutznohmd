/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#include "libhutznohmd/types.hpp"
#include "utility/common.hpp"

using namespace testing;

namespace hutzn
{

using check_range_test_param = std::tuple<size_t, bool>;

class check_range_test : public TestWithParam<check_range_test_param>
{
};

std::vector<check_range_test_param> get_all_check_range_test_params()
{
    std::vector<check_range_test_param> result;
    result.push_back(std::make_tuple(1, false));
    result.push_back(std::make_tuple(2, true));
    result.push_back(std::make_tuple(3, true));
    result.push_back(std::make_tuple(4, false));
    return result;
}

INSTANTIATE_TEST_CASE_P(common, check_range_test,
                        ValuesIn(get_all_check_range_test_params()));

TEST_P(check_range_test, bounds)
{
    size_t input;
    bool output;
    std::tie(input, output) = GetParam();

    auto fn = [](const size_t i) { return check_range<size_t, 2, 3>(i); };
    EXPECT_EQ(output, fn(input));
}

using check_url_test_param = std::tuple<const char_t* const, bool>;

class check_url_test : public TestWithParam<check_url_test_param>
{
};

std::vector<check_url_test_param> get_all_check_url_test_params()
{
    std::vector<check_url_test_param> result;
    result.push_back(std::make_tuple("/foo/bar", true));
    result.push_back(std::make_tuple("/foo/bar/", true));
    result.push_back(std::make_tuple("/", true));
    result.push_back(std::make_tuple("//", false));
    result.push_back(std::make_tuple("/foo//bar", false));
    result.push_back(std::make_tuple("/foo/bar//", false));
    result.push_back(std::make_tuple("foo", false));
    result.push_back(std::make_tuple("foo/bar", false));
    result.push_back(std::make_tuple("/#", false));
    result.push_back(std::make_tuple("/?", false));
    return result;
}

INSTANTIATE_TEST_CASE_P(common, check_url_test,
                        ValuesIn(get_all_check_url_test_params()));

TEST_P(check_url_test, validity)
{
    const char_t* input;
    bool output;
    std::tie(input, output) = GetParam();

    EXPECT_EQ(output, is_valid_url_path(input));
}

} // namespace hutzn
