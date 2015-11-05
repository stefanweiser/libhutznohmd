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
#include <condition_variable>

#include "demux/demultiplexer_ordered_mime_map.hpp"
#include "demux/disconnect_interface.hpp"
#include "demux/usage_interface.hpp"
#include "libhutznohmd/demux.hpp"
#include "request/mime_handler.hpp"

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
class demultiplexer : public demux_interface,
                      public disconnect_interface,
                      public usage_interface
{
public:
    explicit demultiplexer(void);

    //! @copydoc demux_query_interface::determine_request_handler()
    request_handler_holder_pointer determine_request_handler(
        const request_interface& request) override;

    //! @copydoc demux_interface::connect()
    handler_pointer connect(const request_handler_id& id,
                            const request_handler_callback& fn) override;

    //! @copydoc disconnect_interface::disconnect()
    void disconnect(const request_handler_id& id) override;

    //! @copydoc disconnect_interface::disable()
    void disable(const request_handler_id& id) override;

    //! @copydoc disconnect_interface::enable()
    void enable(const request_handler_id& id) override;

    //! @copydoc disconnect_interface::is_enabled()
    bool is_enabled(const request_handler_id& id) const override;

    //! @copydoc demux_interface::register_mime_type()
    mime_type register_mime_type(const std::string& type) override;

    //! @copydoc demux_interface::register_mime_subtype()
    mime_subtype register_mime_subtype(const std::string& subtype) override;

    //! @copydoc demux_interface::unregister_mime_type()
    bool unregister_mime_type(const mime_type& type) override;

    //! @copydoc demux_interface::unregister_mime_subtype()
    bool unregister_mime_subtype(const mime_subtype& subtype) override;

    //! @copydoc usage_interface::increase_usage_counter()
    void increase_usage_counter(const request_handler_id& id) override;

    //! @copydoc usage_interface::decrease_usage_counter()
    void decrease_usage_counter(const request_handler_id& id) override;

private:
    struct resource_key
    {
        std::string path;
        http_verb method;
        mime content_type;

        bool operator<(const resource_key& rhs) const;
    };

    std::tuple<mime, request_handler_callback> lookup_in_accept_map(
        const request_interface& request,
        const demultiplexer_ordered_mime_map& accept_map) const;

    using resource_map = std::map<resource_key, demultiplexer_ordered_mime_map>;

    //! Guards the access to the resource map from mutual access.
    mutable std::mutex resource_callbacks_mutex_;

    //! Gets notified, when the usage flag of a resource got changed.
    mutable std::condition_variable resource_callbacks_usage_changed_;

    //! Stores all request handler callbacks.
    resource_map resource_callbacks_;

    //! Used to store registered mime types and subtypes and to determine
    //! validity.
    mime_handler mime_handler_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_HPP
