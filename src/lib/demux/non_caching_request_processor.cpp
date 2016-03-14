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

#include "non_caching_request_processor.hpp"

#include <cassert>

#include "demux/error_handler.hpp"

namespace hutzn
{

request_processor_ptr make_non_caching_request_processor(
    const demux_query_ptr& query, const uint64_t& connection_timeout_in_sec)
{
    return std::make_shared<non_caching_request_processor>(
        query, connection_timeout_in_sec);
}

non_caching_request_processor::non_caching_request_processor(
    const demux_query_ptr& query, const uint64_t& /*connection_timeout_in_sec*/)
    : query_(query)
    // , connection_timeout_in_sec_(connection_timeout_in_sec)
    , error_handler_mutex_()
    , error_handlers_()
{
}

bool non_caching_request_processor::handle_one_request(
    block_device& /*device*/) const
{
    return false;
}

handler_ptr non_caching_request_processor::set_error_handler(
    const http_status_code& code, const error_handler_callback& fn)
{
    std::lock_guard<std::mutex> lock(error_handler_mutex_);

    // std::map<>::insert will not insert an already inserted element.
    std::pair<error_handler_map::iterator, bool> insertion_result =
        error_handlers_.insert(std::make_pair(code, std::make_tuple(fn, true)));

    handler_ptr result;
    if (insertion_result.second) {
        result = std::make_shared<error_handler>(*this, code);
    }
    return result;
}

void non_caching_request_processor::reset_error_handler(
    const http_status_code& code)
{
    std::lock_guard<std::mutex> lock(error_handler_mutex_);
    const bool erase_result = (error_handlers_.erase(code) > 0);
    assert(erase_result);
    UNUSED(erase_result);
}

void non_caching_request_processor::disable(const http_status_code& code)
{
    std::lock_guard<std::mutex> lock(error_handler_mutex_);
    auto found = error_handlers_.find(code);
    if (found != error_handlers_.end()) {
        std::get<1>(found->second) = false;
    }
}

void non_caching_request_processor::enable(const http_status_code& code)
{
    std::lock_guard<std::mutex> lock(error_handler_mutex_);
    auto found = error_handlers_.find(code);
    if (found != error_handlers_.end()) {
        std::get<1>(found->second) = true;
    }
}

bool non_caching_request_processor::is_enabled(
    const http_status_code& code) const
{
    std::lock_guard<std::mutex> lock(error_handler_mutex_);
    bool result = false;
    auto found = error_handlers_.find(code);
    if (found != error_handlers_.end()) {
        result = std::get<1>(found->second);
    }
    return result;
}

} // namespace hutzn
