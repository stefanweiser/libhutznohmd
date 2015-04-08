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

#include <demux/mock_demux_disconnect_interface.hpp>
#include <demux/demultiplex_handler.hpp>

using namespace testing;

namespace hutzn
{

namespace demux
{

bool operator==(const request_handler_id& lhs, const request_handler_id& rhs)
{
    return ((lhs.path == rhs.path) && (lhs.method == rhs.method) &&
            (lhs.input_type == rhs.input_type) &&
            (lhs.result_type == rhs.result_type));
}

TEST(demultiplexer_handler, disconnect)
{
    demux_disconnect_mock_pointer demuxer =
        std::make_shared<demux_disconnect_interface_mock>();

    request_handler_id id{
        "/", hutzn::request::http_verb::GET,
        hutzn::request::mime(hutzn::request::mime_type::WILDCARD,
                             hutzn::request::mime_subtype::WILDCARD),
        hutzn::request::mime(hutzn::request::mime_type::TEXT,
                             hutzn::request::mime_subtype::PLAIN)};
    EXPECT_CALL(*demuxer, disconnect(id)).Times(1).WillOnce(Return(true));
    demultiplex_handler d(*demuxer, id);
}

} // namespace demux

} // namespace hutzn
