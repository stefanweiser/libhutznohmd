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

#ifndef LIBHUTZNOHMD_DEMUX_REQUEST_PROCESSOR_HPP
#define LIBHUTZNOHMD_DEMUX_REQUEST_PROCESSOR_HPP

#include <map>
#include <memory>
#include <mutex>

#include "demux/reset_error_handler_interface.hpp"
#include "libhutznohmd/demux.hpp"
#include "libhutznohmd/request.hpp"
#include "libhutznohmd/sockets.hpp"

namespace hutzn
{

class request_processor : public request_processor_interface,
                          public reset_error_handler_interface
{
public:
    explicit request_processor(const demux_query_pointer& query_interface,
                               const uint64_t& connection_timeout_in_sec);

    bool handle_one_request(block_device_interface& device) const override;
    handler_pointer set_error_handler(
        const http_status_code& code,
        const error_handler_callback& fn) override;

    void reset_error_handler(const http_status_code& code) override;
    void disable(const http_status_code& code) override;
    void enable(const http_status_code& code) override;
    bool is_enabled(const http_status_code& code) const override;

private:
    using error_handler_map =
        std::map<http_status_code, std::tuple<error_handler_callback, bool>>;

    demux_query_pointer demultiplexer_;
    // uint64_t connection_timeout_in_sec_;

    mutable std::mutex error_handler_mutex_;
    error_handler_map error_handlers_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_REQUEST_PROCESSOR_HPP
