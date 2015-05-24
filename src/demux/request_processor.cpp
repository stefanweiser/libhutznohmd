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

#include <cassert>

#include <demux/error_handler.hpp>

#include "request_processor.hpp"

namespace hutzn
{

request_processor_pointer make_request_processor(
    const demux_query_pointer& query_interface,
    const uint64_t& connection_timeout_in_sec)
{
    return std::make_shared<request_processor>(query_interface,
                                               connection_timeout_in_sec);
}

request_processor::request_processor(const demux_query_pointer& query_interface,
                                     const uint64_t& connection_timeout_in_sec)
    : demultiplexer_(query_interface)
    , connection_timeout_in_sec_(connection_timeout_in_sec)
    , error_handler_mutex_()
    , error_handlers_()
{
}

bool request_processor::handle_one_request(
    block_device_interface& /*device*/) const
{
    return false;
}

handler_pointer request_processor::set_error_handler(
    const http_status_code& code, const error_handler_callback& fn)
{
    std::lock_guard<std::mutex> lock(error_handler_mutex_);

    // std::map<>::insert will not insert an already inserted element.
    std::pair<error_handler_map::iterator, bool> insertion_result =
        error_handlers_.insert(std::make_pair(code, fn));

    handler_pointer result;
    if (true == insertion_result.second) {
        result = std::make_shared<error_handler>(*this, code);
    }
    return result;
}

void request_processor::reset_error_handler(const http_status_code& code)
{
    std::lock_guard<std::mutex> lock(error_handler_mutex_);
    assert(error_handlers_.erase(code) > 0);
}

} // namespace hutzn
