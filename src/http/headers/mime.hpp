/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBREST_HTTP_HEADERS_ACCEPT_HPP__
#define __LIBREST_HTTP_HEADERS_ACCEPT_HPP__

#include <cstdint>
#include <map>
#include <string>

namespace rest
{

namespace http
{

class MimeType
{
public:
    enum class Type
    {
        Wildcard

    };

    enum class Subtype
    {
        Wildcard
    };

    explicit MimeType(const std::string & string);

private:
    const std::string m_string;
    Type m_type;
    Subtype m_subtype;
    uint16_t m_quality;
    std::map<std::string, std::string> m_extensions;
};

std::istream & operator>>(std::istream & is, MimeType & type);

} // namespace http

} // namespace rest

#endif // __LIBREST_HTTP_HEADERS_ACCEPT_HPP__
