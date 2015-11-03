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

#ifndef LIBHUTZNOHMD_DEMUX_ERROR_HANDLER_HPP
#define LIBHUTZNOHMD_DEMUX_ERROR_HANDLER_HPP

#include <memory>

#include "demux/reset_error_handler_interface.hpp"
#include "libhutznohmd/demux.hpp"

namespace hutzn
{

class error_handler : public handler_interface
{
public:
    explicit error_handler(reset_error_handler_interface& request_processor,
                           const http_status_code& code);

    ~error_handler(void) noexcept(true) override;
    void disable(void) override;
    void enable(void) override;
    bool is_enabled(void) const override;

private:
    reset_error_handler_interface& request_processor_;

    http_status_code code_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_ERROR_HANDLER_HPP
