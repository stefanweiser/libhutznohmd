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

#include "libhutznohmd/demux.hpp"

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
    //! @brief Constructs a ordered MIME map for the use in a demultiplexer.
    explicit demultiplexer_ordered_mime_map(void);

    //! @brief Returns the number of elements stored in the data structure.
    size_t size(void) const;

    //! @brief Inserts a new value into the data structure.
    //!
    //! Returns @c true when the element was inserted successfully. It is not
    //! allowed to insert wildcard elements to the data structure.
    bool insert(const mime& type, const request_handler_callback& fn);

    //! @brief Erases a previously stored element from the data structure.
    //!
    //! Returns @c true, when the element was successfully erased.
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
    //! The input mime type may be altered, if some wildcard resolving was done.
    //! It contains the concrete mime type of the request handler afterwards.
    request_handler_callback find(mime& type) const;

    //! Increases the handlers usage counter. If the counter is greater than
    //! zero, it will prevent the erase operation to succeed.
    void increase_usage_counter(const mime& type);

    //! Decreases the handlers usage counter. If the counter is greater than
    //! zero, it will prevent the erase operation to succeed.
    void decrease_usage_counter(const mime& type);

    //! Returns true, when the handler is currently in use.
    bool is_used(const mime& type) const;

    //! Sets whether the handler is available to get found. Setting it false
    //! will prevent the find operation to return that handler.
    void set_availability(const mime& type, const bool available);

    //! Returns true, when the handler is currently available.
    bool is_available(const mime& type) const;

private:
    struct request_handler_infos
    {
        request_handler_callback handler;
        size_t usage_counter;
        bool is_available;
    };

    //! Tries to find and returns a matching request handler for a given mime
    //! type respecting the insertion order. Wildcards are treated correctly as
    //! any element while searching. The type may be altered when a request
    //! handler was found. It contains the concrete mime type of the returned
    //! value after the call in that case.
    request_handler_callback find_ordered(mime& type) const;

    //! Stores the request handler callbacks by its mime type keys.
    std::map<mime, request_handler_infos> map_;

    //! Stores the insertion order.
    std::vector<mime> vector_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_ORDERED_MIME_MAP_HPP
