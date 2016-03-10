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

//! Provides helper logic to process requests.
class request_processor : public request_processor_interface,
                          public reset_error_handler_interface
{
public:
    //! Constructs a request processor.
    explicit request_processor(const demux_query_ptr& query_interface,
                               const uint64_t& connection_timeout_in_sec);

    //! @copydoc request_processor_interface::handle_one_request()
    bool handle_one_request(block_device& device) const override;

    //! @copydoc request_processor_interface::set_error_handler()
    handler_ptr set_error_handler(const http_status_code& code,
                                  const error_handler_callback& fn) override;

    //! @copydoc reset_error_handler_interface::reset_error_handler()
    void reset_error_handler(const http_status_code& code) override;

    //! @copydoc reset_error_handler_interface::disable()
    void disable(const http_status_code& code) override;

    //! @copydoc reset_error_handler_interface::enable()
    void enable(const http_status_code& code) override;

    //! @copydoc reset_error_handler_interface::is_enabled()
    bool is_enabled(const http_status_code& code) const override;

private:
    using error_handler_map =
        std::map<http_status_code, std::tuple<error_handler_callback, bool>>;

    demux_query_ptr demultiplexer_;

    mutable std::mutex error_handler_mutex_;
    error_handler_map error_handlers_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_REQUEST_PROCESSOR_HPP
