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

#ifndef LIBHUTZNOHMD_REQUEST_MIME_HANDLER_HPP
#define LIBHUTZNOHMD_REQUEST_MIME_HANDLER_HPP

#include <mutex>

#include <libhutznohmd/request.hpp>

#include <request/mime_data.hpp>

namespace hutzn
{

class mime_handler
{
public:
    //! @brief The only way to construct a mime handler.
    explicit mime_handler(void);

    //! @brief Registers a string as mime type. Returns the resulting mime type.
    mime_type register_mime_type(const std::string& type);

    //! @brief Registers a string as mime subtype. Returns the resulting mime
    //! subtype.
    mime_subtype register_mime_subtype(const std::string& subtype);

    //! @brief Unregisters a mime type. Returns true if the unregistering was
    //! successful.
    bool unregister_mime_type(const mime_type& type);

    //! @brief Unregisters a mime subtype. Returns true if the unregistering was
    //! successful.
    bool unregister_mime_subtype(const mime_subtype& subtype);

    //! @brief Determines, whether two types are valid or not.
    //!
    //! Returns true, when both types are valid ones and false in any other
    //! case.
    bool are_two_types_valid(const mime& type1, const mime& type2) const;

    //! @brief Parses a complete mime.
    //!
    //! Takes a string with a fixed size, which may not be null-terminated.
    //! Returns the parsed mime type or invalid type and subtype when the string
    //! does not exactly match a registered mime type or subtype.
    mime parse(const char_t* const data, const size_t max_length) const;

private:
    //! @brief Determines, whether a mime type is valid or not.
    //!
    //! Checks whether a type and subtype is consequently none or not. No part
    //! must be invalid and both parts must be registered, when not none.
    bool is_mime_valid(const mime& t) const;

    //! Guards the request parser data from mutual access.
    mutable std::mutex mime_type_mutex_;

    //! Stores registered mime types.
    mime_data<mime_type, uint8_t> mime_types_;

    //! Stores registered mime subtypes.
    mime_data<mime_subtype, uint16_t> mime_subtypes_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_MIME_HANDLER_HPP
