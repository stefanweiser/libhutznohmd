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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_DEMUX_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_DEMUX_HPP

#include <gmock/gmock.h>

#include <demux/demux_disconnect_interface.hpp>

namespace hutzn
{

class demux_disconnect_interface_mock : public demux_disconnect_interface
{
public:
    MOCK_METHOD1(disconnect, bool(const request_handler_id&));
};

using demux_disconnect_mock_pointer =
    std::shared_ptr<demux_disconnect_interface_mock>;

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_DEMUX_HPP
