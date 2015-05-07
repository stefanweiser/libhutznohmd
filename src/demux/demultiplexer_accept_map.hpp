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

#ifndef LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ACCEPT_MAP_HPP
#define LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ACCEPT_MAP_HPP

#include <map>

#include <libhutznohmd/demux.hpp>

namespace hutzn
{

//! Stores request handler callbacks by a mime type of the accept header as key.
//! All request handler callbacks are stored in its insertion order. Therefore
//! it implements a first-come-first-serve idea, when multiple handlers match
//! the accept headers precedence. This data structure is not multi threading
//! safe. It does not handle special mime types like invalid or unset ones.
class demultiplexer_accept_map
{
public:
    explicit demultiplexer_accept_map();

    //! Returns the number of elements stored in the data structure.
    size_t size() const;

    //! Inserts a new value into the data structure. Returns true when the
    //! element was inserted successfully. It is not allowed to insert wildcard
    //! elements to the data structure.
    bool insert(const mime& type, const request_handler_callback& fn);

    //! Erases a previously stored element from the data structure and returns
    //! true, when the element was successfully erased.
    bool erase(const mime& type);

    //! Negotiates the most matching request handler for a given mime type. The
    //! function returns the first matching request handler. If a type contains
    //! a wildcard, the insertion order is respected.
    //!
    //! As an example, the following mime types are given inserted from left to
    //! right:
    //! @code{.unparsed}
    //! application/xml, text/xml, text/plain
    //! @endcode
    //! While @c find(text/plain) will return the handler of @c text/plain,
    //! @c find(text/*) will return the handler of @c text/xml. @c find(*/xml)
    //! will return the handler of @c application/xml.
    request_handler_callback find(const mime& type) const;

private:
    //! Tries to find and returns a matching request handler for a given mime
    //! type respecting the insertion order. Wildcards are treated correctly as
    //! any element while searching.
    request_handler_callback find_ordered(const mime& type) const;

    //! Stores the request handler callbacks by its mime type keys.
    std::map<mime, request_handler_callback> map_;

    //! Stores the insertion order.
    std::vector<mime> vector_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ACCEPT_MAP_HPP
