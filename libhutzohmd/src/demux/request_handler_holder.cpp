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

#include "request_handler_holder.hpp"

namespace hutzn
{

request_handler_holder::request_handler_holder(
    usage& demuxer, const request_handler_id& id,
    const request_handler_callback& callback)
    : demuxer_(demuxer)
    , id_(id)
    , callback_(callback)
{
    demuxer_.increase_usage_counter(id_);
}

request_handler_holder::~request_handler_holder(void) noexcept(true)
{
    demuxer_.decrease_usage_counter(id_);
}

http_status_code request_handler_holder::call(request& request,
                                              response& response)
{
    const http_status_code result = callback_(request, response);
    return result;
}

} // namespace hutzn
