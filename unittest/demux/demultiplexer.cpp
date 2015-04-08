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

#include <demux/demultiplexer.hpp>

using namespace testing;

namespace hutzn
{

namespace demux
{

TEST(demultiplexer, construction)
{
    demux_pointer demultiplexer = make_demultiplexer();
    EXPECT_NE(demultiplexer.get(), nullptr);
}

TEST(demultiplexer, connect_disconnect)
{
    demux_pointer demultiplexer = make_demultiplexer();
    ASSERT_NE(demultiplexer.get(), nullptr);

    request_handler_id id{
        "/", hutzn::request::http_verb::GET,
        hutzn::request::mime(hutzn::request::mime_type::WILDCARD,
                             hutzn::request::mime_subtype::WILDCARD),
        hutzn::request::mime(hutzn::request::mime_type::TEXT,
                             hutzn::request::mime_subtype::PLAIN)};
    auto fn = [](const hutzn::request::request_interface&,
                 hutzn::request::response_interface&) {
        return hutzn::request::http_status_code::OK;
    };
    handler_pointer handler1 = demultiplexer->connect(id, fn);

    EXPECT_NE(handler1.get(), nullptr);
}

TEST(demultiplexer, connect_twice)
{
    demux_pointer demultiplexer = make_demultiplexer();
    ASSERT_NE(demultiplexer.get(), nullptr);

    request_handler_id id{
        "/", hutzn::request::http_verb::GET,
        hutzn::request::mime(hutzn::request::mime_type::WILDCARD,
                             hutzn::request::mime_subtype::WILDCARD),
        hutzn::request::mime(hutzn::request::mime_type::TEXT,
                             hutzn::request::mime_subtype::PLAIN)};
    auto fn = [](const hutzn::request::request_interface&,
                 hutzn::request::response_interface&) {
        return hutzn::request::http_status_code::OK;
    };
    handler_pointer handler1 = demultiplexer->connect(id, fn);
    handler_pointer handler2 = demultiplexer->connect(id, fn);

    EXPECT_NE(handler1.get(), nullptr);
    EXPECT_EQ(handler2.get(), nullptr);
}

} // namespace demux

} // namespace hutzn
