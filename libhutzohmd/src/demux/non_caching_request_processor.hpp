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

#ifndef LIBHUTZNOHMD_DEMUX_NON_CACHING_REQUEST_PROCESSOR_HPP
#define LIBHUTZNOHMD_DEMUX_NON_CACHING_REQUEST_PROCESSOR_HPP

#include <map>
#include <memory>
#include <mutex>

#include "demux/error_handler_manager.hpp"
#include "libhutznohmd/communication.hpp"
#include "libhutznohmd/demux.hpp"
#include "libhutznohmd/request.hpp"

namespace hutzn
{

//! Provides helper logic to process requests.
class non_caching_request_processor : public request_processor,
                                      public error_handler_manager
{
public:
    //! Constructs a request processor.
    explicit non_caching_request_processor(
        const demux_query_ptr& query,
        const uint64_t& connection_timeout_in_sec);

    //! @copydoc request_processor::handle_one_request()
    bool handle_one_request(block_device& device) const override;

    //! @copydoc request_processor::set_error_handler()
    handler_ptr set_error_handler(const http_status_code& code,
                                  const error_handler_callback& fn) override;

    //! @copydoc error_handler_manager::reset_error_handler()
    void reset_error_handler(const http_status_code& code) override;

    //! @copydoc error_handler_manager::disable()
    void disable(const http_status_code& code) override;

    //! @copydoc error_handler_manager::enable()
    void enable(const http_status_code& code) override;

    //! @copydoc error_handler_manager::is_enabled()
    bool is_enabled(const http_status_code& code) const override;

private:
    using error_handler_map =
        std::map<http_status_code, std::tuple<error_handler_callback, bool>>;

    demux_query_ptr query_;

    mutable std::mutex error_handler_mutex_;
    error_handler_map error_handlers_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_NON_CACHING_REQUEST_PROCESSOR_HPP
