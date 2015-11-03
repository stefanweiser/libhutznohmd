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

#ifndef LIBHUTZNOHMD_REQUEST_REQUEST_HPP
#define LIBHUTZNOHMD_REQUEST_REQUEST_HPP

#include <map>

#include "libhutznohmd/request.hpp"
#include "request/lexer.hpp"
#include "request/mime_handler.hpp"
#include "request/uri.hpp"

namespace hutzn
{

enum class header_key : int8_t {
    CUSTOM = -1,
    CONNECTION,
    CONTENT_LENGTH,
    CONTENT_TYPE,
    DATE,
    EXPECT,
    FROM,
    REFERER,
    USER_AGENT,
    SIZE
};

class request : public request_interface
{
public:
    explicit request(const request& rhs) = delete;
    request& operator=(const request& rhs) = delete;

    explicit request(const connection_pointer& connection);

    bool parse(const mime_handler& handler);

    void fetch_content(void) override;
    http_verb method(void) const override;
    const char_t* path(void) const override;
    const char_t* host(void) const override;
    const char_t* query(const char_t* const key) const override;
    const char_t* fragment(void) const override;
    http_version version(void) const override;
    const char_t* header_value(const char_t* const name) const override;
    bool keeps_connection(void) const override;
    time_t date(void) const override;
    const void* content(void) const override;
    size_t content_length(void) const override;
    mime content_type(void) const override;
    bool accept(void*& handle, mime& type) const override;
    http_expectation expect(void) const override;
    const char_t* from(void) const override;
    const char_t* referer(void) const override;
    const char_t* user_agent(void) const override;

private:
    template <typename T>
    bool parse_specific_value(const trie<T>& t, const size_t& max_length,
                              const char_t* const string, const size_t& length,
                              T& value);

    bool parse_method(int32_t& ch);
    bool parse_uri(int32_t& ch);
    bool parse_version(int32_t& ch);

    //! Parses a header utilizing the lexer member. Returns true, if a header
    //! could successfully get parsed. Returning false means, that the lexer has
    //! reached the end of the file. The in/out parameter c is -1 in this case.
    bool parse_header(const mime_handler& handler, int32_t& ch);

    void set_header(const mime_handler& handler, header_key key,
                    const char_t* const key_string, const char_t* value_string,
                    size_t value_length);

    void set_connection(const mime_handler& handler, const char_t* value_string,
                        size_t value_length);

    void set_content_length(const mime_handler& handler,
                            const char_t* value_string, size_t value_length);

    void set_content_type(const mime_handler& handler,
                          const char_t* value_string, size_t value_length);

    void set_date(const mime_handler& handler, const char_t* value_string,
                  size_t value_length);

    void set_expect(const mime_handler& handler, const char_t* value_string,
                    size_t value_length);

    void set_from(const mime_handler& handler, const char_t* value_string,
                  size_t value_length);

    void set_referer(const mime_handler& handler, const char_t* value_string,
                     size_t value_length);

    void set_user_agent(const mime_handler& handler, const char_t* value_string,
                        size_t value_length);

    static bool is_whitespace(const int32_t ch);
    static bool is_newline(const int32_t ch);
    static bool is_key_value_separator(const int32_t ch);

    lexer lexer_;

    http_verb method_;
    uri path_uri_;
    http_version version_;

    size_t content_length_;
    mime content_type_;
    const void* content_;
    bool is_keep_alive_set_;
    time_t date_;
    http_expectation expect_;
    const char_t* from_;
    const char_t* referer_;
    const char_t* user_agent_;

    //! Do not use this in any other case than for storing header fields and
    //! query entries! It is designed for only one purpose: To let the string's
    //! content decide the order of the elements in the map. This map has to
    //! enforce, that all characters are null-terminated!
    struct enforced_null_terminated_less
    {
        bool operator()(const char_t* const lhs, const char_t* const rhs) const;
    };

    std::map<const char_t* const, const char_t*, enforced_null_terminated_less>
        header_fields_;
    std::map<const char_t* const, const char_t*, enforced_null_terminated_less>
        query_entries_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_REQUEST_HPP
