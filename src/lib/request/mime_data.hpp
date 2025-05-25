/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_REQEUST_MIME_DATA_HPP
#define LIBHUTZNOHMD_REQEUST_MIME_DATA_HPP

#include <map>
#include <string>
#include <type_traits>

#include "utility/trie.hpp"

namespace hutzn
{

//! @brief Stores mime classes.
//!
//! The classes are for example the types 'audio', 'video' or the subtypes
//! 'plain'. They has to be registered first and finally they has to get
//! unregistered. It takes a value type, which has to be an enum or enum class
//! and has a value named INVALID.
template <typename value_type>
class mime_data
{
public:
    //! @brief Constructs an empty structure.
    explicit mime_data(void)
        : next_value_(special_value_count)
        , registered_types_()
        , types_(true)
    {
        // registering the wildcard value
        const value_type type = register_type("*");
        assert(type == value_type::WILDCARD);
        UNUSED(type);
    }

    //! @brief Registers a type.
    //!
    //! The type string must not be empty. Returns the registration value.
    //! @param[in] type String to register.
    //! @return         Registration value.
    value_type register_type(const std::string& type)
    {
        value_type result = value_type::INVALID;
        if ((type.size() > 0) && (next_value_ != 0)) {
            value_type value = static_cast<value_type>(next_value_);
            if (types_.insert(type.c_str(), value)) {
                next_value_++;
                registered_types_[value] = type;
                result = value;
            }
        }

        return result;
    }

    //! @brief Unregisters a type.
    //!
    //! Returns true on success.
    //! @param[in] type Registration value to unregister.
    //! @return         True on success and else when unregistration failed.
    bool unregister_type(const value_type& type)
    {
        bool result;
        auto it = registered_types_.find(type);
        if (it != registered_types_.end()) {
            const bool erase_result = types_.erase(it->second.c_str());

            // the registration of that type is based on the existence in the
            // trie and not in the map
            // therefore ignoring this result is mandatory
            registered_types_.erase(it);
            result = erase_result;
        } else {
            result = false;
        }
        return result;
    }

    //! @brief Parses a string to find a registered value.
    //!
    //! Returns the parsed registration value or INVALID when not.
    //! @param[in] string String to search on.
    //! @param[in] expected_length Length of the string to search on.
    //! @return           A registrated value or INVALID.
    value_type parse_type(const char_t* const string,
                          const size_t expected_length) const
    {
        value_type result;
        auto found_element = types_.find(string, expected_length);
        if (found_element.used_size() == expected_length) {
            result = found_element.value();
        } else {
            result = value_type::INVALID;
        }
        return result;
    }

    //! @brief Checks whether a type is registered or not.
    //!
    //! Returns true when it is registered and else when not.
    //! @param[in] type Type to check.
    //! @return         True when type is registered and false when not.
    bool is_registered(const value_type& type) const
    {
        return registered_types_.find(type) != registered_types_.end();
    }

private:
    //! Underlying type of the enum is required to calculate new registration
    //! values.
    using arithmetic_type = typename std::underlying_type<value_type>::type;

    //! There are always two special values: INVALID and NONE. Therefore
    //! indexing starts with 2.
    static const arithmetic_type special_value_count = 2;

    //! Stores the value returned on next registration request.
    arithmetic_type next_value_;

    //! A map to find the values stored in the trie by its value_type.
    std::map<value_type, std::string> registered_types_;

    //! Stores the values and the way to them.
    trie<value_type> types_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQEUST_MIME_DATA_HPP
