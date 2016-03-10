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

#ifndef LIBHUTZNOHMD_DEMUX_REQUEST_HANDLER_HOLDER_HPP
#define LIBHUTZNOHMD_DEMUX_REQUEST_HANDLER_HOLDER_HPP

#include <memory>

#include "demux/usage_interface.hpp"
#include "libhutznohmd/demux.hpp"

namespace hutzn
{

//! Stores a request handler to call it.
class request_handler_holder : public callback_holder
{
public:
    //! Constructs a request handler holder.
    explicit request_handler_holder(usage_interface& demuxer,
                                    const request_handler_id& id,
                                    const request_handler_callback& callback);

    //! @copydoc
    //! request_handler_holder_interface::~request_handler_holder_interface()
    ~request_handler_holder(void) noexcept(true);

    //! @copydoc request_handler_holder_interface::call()
    http_status_code call(request_interface& request,
                          response_interface& response) override;

private:
    usage_interface& demuxer_;
    request_handler_id id_;
    request_handler_callback callback_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_REQUEST_HANDLER_HOLDER_HPP
