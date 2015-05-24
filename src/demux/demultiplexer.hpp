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

#ifndef LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_HPP
#define LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_HPP

#include <memory>
#include <mutex>

#include <libhutznohmd/demux.hpp>

#include <request/parser_data.hpp>
#include <demux/demultiplexer_ordered_mime_map.hpp>

#include <demux/disconnect_interface.hpp>

namespace hutzn
{

//! @brief Implements logic to determine the right request handler for a given
//! request.
//!
//! Handles a request handler and a mime type and subtype storage. Implements
//! all necessary interfaces to expose access to mime types, request handler
//! management and the request handler determination algorithm. This class is
//! designed to be thread safe. It does not call any callbacks into the user's
//! domain.
class demultiplexer : public demux_interface, public disconnect_interface
{
public:
    explicit demultiplexer(void);

    request_handler_callback determine_request_handler(
        const request_interface& request) const override;
    handler_pointer connect(const request_handler_id& id,
                            const request_handler_callback& fn) override;
    void disconnect(const request_handler_id& id) override;
    mime_type register_mime_type(const std::string& type) override;
    mime_subtype register_mime_subtype(const std::string& subtype) override;
    bool unregister_mime_type(const mime_type& type) override;
    bool unregister_mime_subtype(const mime_subtype& subtype) override;

private:
    struct resource_key
    {
        std::string path;
        http_verb method;
        mime content_type;

        bool operator<(const resource_key& rhs) const;
    };

    using resource_map = std::map<resource_key, demultiplexer_ordered_mime_map>;

    //! @brief Determines, whether a mime type is valid or not.
    //!
    //! Checks whether a type and subtype is consequently none or not. No part
    //! must be invalid and both parts must be registered, when not none.
    bool is_mime_valid(const mime& t) const;

    //! Guards the access to the resource map from mutual access.
    mutable std::mutex resource_callbacks_mutex_;

    //! Stores all request handler callbacks.
    resource_map resource_callbacks_;

    //! Guards the request parser data from mutual access.
    std::mutex request_parser_data_mutex_;

    //! Stores registered mime types.
    parser_data request_parser_data_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_HPP
