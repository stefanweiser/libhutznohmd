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

#ifndef LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ORDERED_MIME_MAP_HPP
#define LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ORDERED_MIME_MAP_HPP

#include <map>

#include <libhutznohmd/demux.hpp>

namespace hutzn
{

//! @brief Associates mime types with request handler callbacks.
//!
//! Stores request handler callbacks by a mime type of the accept header as key.
//! All request handler callbacks are stored in its insertion order. Therefore
//! it implements a first-come-first-serve idea, when multiple handlers match
//! the accept headers precedence. This data structure is not multi threading
//! safe. It does not handle special mime types like invalid or unset ones.
class demultiplexer_ordered_mime_map
{
public:
    explicit demultiplexer_ordered_mime_map();

    //! @brief Returns the number of elements stored in the data structure.
    size_t size() const;

    //! @brief Inserts a new value into the data structure.
    //!
    //! Returns true when the element was inserted successfully. It is not
    //! allowed to insert wildcard elements to the data structure.
    bool insert(const mime& type, const request_handler_callback& fn);

    //! @brief Erases a previously stored element from the data structure.
    //!
    //! Returns true, when the element was successfully erased.
    bool erase(const mime& type);

    //! @brief Negotiates the most matching request handler for a given mime
    //! type.
    //!
    //! The function returns the first matching request handler. If a type
    //! contains a wildcard, the insertion order is respected. As an example,
    //! the following mime types are given as inserted from left to right:
    //! @code{.unparsed}
    //! insert(application/xml);
    //! insert(text/xml);
    //! insert(text/plain);
    //!
    //! find(text/plain) returns handler of text/plain;
    //! find(text/ *) returns handler of text/xml;
    //! find(* /xml) returns handler of application/xml;
    //! @endcode
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

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ORDERED_MIME_MAP_HPP
