/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_DEMUX_MOCK_DISCONNECT_INTERFACE_HPP
#define LIBHUTZNOHMD_DEMUX_MOCK_DISCONNECT_INTERFACE_HPP

#include <gmock/gmock.h>

#include "demux/handler_manager.hpp"

namespace hutzn
{

class handler_manager_mock : public handler_manager
{
public:
    MOCK_METHOD(void, disconnect, (const request_handler_id&));
    MOCK_METHOD(void, disable, (const request_handler_id&));
    MOCK_METHOD(void, enable, (const request_handler_id&));
    MOCK_METHOD(bool, is_enabled, (const request_handler_id&), (const));
};

using handler_manager_mock_ptr = std::shared_ptr<handler_manager_mock>;

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_MOCK_DISCONNECT_INTERFACE_HPP
