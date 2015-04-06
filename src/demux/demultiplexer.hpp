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

namespace hutzn
{

namespace demux
{

class demultiplexer : public demux_interface
{
public:
    explicit demultiplexer();

    request_handler_callback determine_request_handler(
        const hutzn::request::request_interface& request) override;

    handler_pointer connect(const request_handler_id& id,
                            const request_handler_callback& fn) override;

    bool disconnect(const request_handler_id& id);

    hutzn::request::mime_type register_mime_type(
        const std::string& type) override;

    hutzn::request::mime_subtype register_mime_subtype(
        const std::string& subtype) override;

    bool unregister_mime_type(const hutzn::request::mime_type& type) override;

    bool unregister_mime_subtype(
        const hutzn::request::mime_subtype& subtype) override;

private:
    using input_result_mime_tuple =
        std::tuple<hutzn::request::mime, hutzn::request::mime>;
    using resource_mime_map =
        std::map<input_result_mime_tuple, request_handler_callback>;
    using resource_method_map =
        std::map<hutzn::request::http_verb, resource_mime_map>;
    using resource_map = std::map<std::string, resource_method_map>;

    std::mutex resource_callbacks_mutex_;
    resource_map resource_callbacks_;

    std::mutex request_parser_data_mutex_;
    hutzn::request::parser_data_pointer request_parser_data_;
};

} // namespace demux

} // namespace hutzn

#endif // LIBHUTZNOHMD_DEMUX_DEMULTIPLEXER_HPP
