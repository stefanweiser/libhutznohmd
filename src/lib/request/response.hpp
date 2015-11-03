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

#ifndef LIBHUTZNOHMD_REQUEST_RESPONSE_HPP
#define LIBHUTZNOHMD_REQUEST_RESPONSE_HPP

#include "libhutznohmd/request.hpp"

namespace hutzn
{

class response : public response_interface
{
public:
    explicit response(const connection_pointer& connection);
    bool set_header(const char_t* const name,
                    const char_t* const value) override;
    void set_content(const buffer& content, const bool set_md5) override;
    void set_content_location(const char_t* const content_location) override;
    void set_location(const char_t* const location) override;
    bool set_retry_after(const time_t retry_time) override;
    void set_server(const char_t* const fingerprint) override;

private:
    connection_pointer connection_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_RESPONSE_HPP
