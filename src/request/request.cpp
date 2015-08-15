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

#include "request.hpp"

namespace hutzn
{

request::request(const connection_pointer& connection)
    : lexer_(connection)
{
}

bool request::fetch_header()
{
    while (-1 == lexer_.get()) {
    }
    return true;
}

http_verb request::method(void) const
{
    return http_verb::GET;
}

const char_t* request::path(void) const
{
    return nullptr;
}

const char_t* request::host(void) const
{
    return nullptr;
}

const char_t* request::query(const char_t* const /*key*/) const
{
    return nullptr;
}

const char_t* request::fragment(void) const
{
    return nullptr;
}

http_version request::version(void) const
{
    return http_version::HTTP_UNKNOWN;
}

const char_t* request::header_value(const char_t* const /*name*/) const
{
    return nullptr;
}

bool request::keeps_connection(void) const
{
    return false;
}

time_t request::date(void) const
{
    return 0;
}

void* request::content(void) const
{
    return nullptr;
}

size_t request::content_length(void) const
{
    return 0;
}

mime request::content_type(void) const
{
    return mime(mime_type::INVALID, mime_subtype::INVALID);
}

bool request::accept(void*& /*handle*/, mime& /*type*/) const
{
    return false;
}

http_expectation request::expect(void) const
{
    return http_expectation::UNKNOWN;
}

const char_t* request::from(void) const
{
    return nullptr;
}

const char_t* request::referer(void) const
{
    return nullptr;
}

const char_t* request::user_agent(void) const
{
    return nullptr;
}

} // namespace hutzn
