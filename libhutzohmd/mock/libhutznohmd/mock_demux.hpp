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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_DEMUX_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_DEMUX_HPP

#include <gmock/gmock.h>

#include "libhutznohmd/demux.hpp"

namespace hutzn
{

class demux_mock : public demux
{
public:
    MOCK_METHOD1(determine_request_handler,
                 callback_holder_ptr(const request&));
    MOCK_METHOD2(connect, handler_ptr(const request_handler_id&,
                                      const request_handler_callback&));
    MOCK_METHOD1(register_mime_type, mime_type(const std::string&));
    MOCK_METHOD1(register_mime_subtype, mime_subtype(const std::string&));
    MOCK_METHOD1(unregister_mime_type, bool(const mime_type&));
    MOCK_METHOD1(unregister_mime_subtype, bool(const mime_subtype&));
};

using demux_mock_ptr = std::shared_ptr<demux_mock>;

class request_processor_mock : public request_processor
{
public:
    MOCK_CONST_METHOD1(handle_one_request, bool(block_device&));
    MOCK_METHOD2(set_error_handler, handler_ptr(const http_status_code&,
                                                const error_handler_callback&));
};

using request_processor_mock_ptr = std::shared_ptr<request_processor_mock>;

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_MOCK_DEMUX_HPP
