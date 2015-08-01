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

#ifndef LIBHUTZNOHMD_REQUEST_REQUEST_HPP
#define LIBHUTZNOHMD_REQUEST_REQUEST_HPP

#include <libhutznohmd/request.hpp>

namespace hutzn
{

class request : public request_interface
{
public:
    explicit request(const connection_pointer& connection);
    http_verb method(void) const override;
    const char_t* path(void) const override;
    const char_t* host(void) const override;
    const char_t* query(const char_t* const key) const override;
    const char_t* fragment(void) const override;
    http_version version(void) const override;
    const char_t* header_value(const char_t* const name) const override;
    bool keeps_connection(void) const override;
    time_t date(void) const override;
    void* content(void) const override;
    size_t content_length(void) const override;
    mime content_type(void) const override;
    bool accept(void*& handle, mime& type) const override;
    http_expectation expect(void) const override;
    const char_t* from(void) const override;
    const char_t* referer(void) const override;
    const char_t* user_agent(void) const override;

private:
    connection_pointer connection_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_REQUEST_HPP
