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

#ifndef LIBHUTZNOHMD_REQEUST_MIME_DATA_HPP
#define LIBHUTZNOHMD_REQEUST_MIME_DATA_HPP

#include <map>
#include <string>

#include <utility/trie.hpp>

namespace hutzn
{

template <typename value_type, typename arithmetic_type>
class mime_data
{
public:
    // There are always two special values: INVALID and NONE. Therefore indexing
    // starts with 2.
    static constexpr arithmetic_type special_value_count = 2;

    static_assert(sizeof(value_type) == sizeof(arithmetic_type),
                  "Value and arithmetic type is not of the same size.");

    explicit mime_data(void)
        : next_value_(special_value_count)
        , registered_types_()
        , types_(true)
    {
    }

    value_type register_type(const std::string& type)
    {
        if (0 == next_value_) {
            return value_type::INVALID;
        }

        value_type value = static_cast<value_type>(next_value_);
        if (true == types_.insert(type.c_str(), value)) {
            next_value_++;
            registered_types_[value] = type;
            return value;
        }
        return value_type::INVALID;
    }

    bool unregister_type(const value_type& type)
    {
        auto it = registered_types_.find(type);
        if (it != registered_types_.end()) {
            const bool result = types_.erase(it->second.c_str());

            // The registration of that type is based on the existence in the
            // trie and not in the map. Therefore ignoring this result is
            // mandatory.
            registered_types_.erase(it);
            return result;
        }
        return false;
    }

    value_type parse_type(const char* const string, const size_t max_length)
    {
        auto result = types_.find(string, max_length);
        if (result.used_size > 0) {
            return result.value;
        }
        return value_type::INVALID;
    }

    bool is_registered(const value_type& type) const
    {
        return (registered_types_.find(type) != registered_types_.end());
    }

private:
    arithmetic_type next_value_;
    std::map<value_type, std::string> registered_types_;
    trie<value_type> types_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQEUST_MIME_DATA_HPP
