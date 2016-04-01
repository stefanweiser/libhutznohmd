/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#ifndef LIBHUTZNOHMD_REQUEST_MEMORY_ALLOCATING_REQUEST_HPP
#define LIBHUTZNOHMD_REQUEST_MEMORY_ALLOCATING_REQUEST_HPP

#include <map>

#include "libhutznohmd/request.hpp"
#include "request/lexer.hpp"
#include "request/mime_handler.hpp"
#include "request/uri.hpp"

namespace hutzn
{

//! Defines the kind of the header.
enum class header_key : int8_t {
    //! Defines a customized kind that must contain the concrete string.
    CUSTOM = -1,

    //! Connection kind.
    CONNECTION,

    //! Content-Length kind.
    CONTENT_LENGTH,

    //! Conent-MD5 kind.
    CONTENT_MD5,

    //! Connection-Type kind.
    CONTENT_TYPE,

    //! Date kind.
    DATE,

    //! Expect kind.
    EXPECT,

    //! From kind.
    FROM,

    //! Host kind.
    HOST,

    //! Referer kind.
    REFERER,

    //! User-Agent kind.
    USER_AGENT,

    //! Number of concrete header kinds.
    SIZE
};

//! @copydoc request
class memory_allocating_request : public request
{
public:
    //! @brief Constructs a request by a connection.
    //!
    //! @param[in] connection Connection to use when more data is needed.
    explicit memory_allocating_request(const connection_ptr& connection);

    explicit memory_allocating_request(const memory_allocating_request& rhs) =
        delete;
    memory_allocating_request& operator=(const memory_allocating_request& rhs) =
        delete;

    //! @brief Parses a request.
    //!
    //! Reads the header from the stream. Splits and converts the header's parts
    //! to improve operation speed afterwards. Needs a mime_handler to be used
    //! when reading MIMEs.
    //! @param[in] handler MIME handler to be used when parsing the header.
    //! @return            True then parsing was successful and false when not.
    bool parse(const mime_handler& handler);

    //! @copydoc request::fetch_content()
    bool fetch_content(void) override;

    //! @copydoc request::method()
    http_verb method(void) const override;

    //! @copydoc request::path()
    const char_t* path(void) const override;

    //! @copydoc request::host()
    const char_t* host(void) const override;

    //! @copydoc request::query()
    const char_t* query(const char_t* const key) const override;

    //! @copydoc request::fragment()
    const char_t* fragment(void) const override;

    //! @copydoc request::version()
    http_version version(void) const override;

    //! @copydoc request::header_value()
    const char_t* header_value(const char_t* const name) const override;

    //! @copydoc request::keeps_connection()
    bool keeps_connection(void) const override;

    //! @copydoc request::date()
    time_t date(void) const override;

    //! @copydoc request::content()
    const void* content(void) const override;

    //! @copydoc request::content_length()
    size_t content_length(void) const override;

    //! @copydoc request::content_type()
    mime content_type(void) const override;

    //! @copydoc request::accept()
    bool accept(void*& handle, mime& type) const override;

    //! @copydoc request::expect()
    http_expectation expect(void) const override;

    //! @copydoc request::from()
    const char_t* from(void) const override;

    //! @copydoc request::referer()
    const char_t* referer(void) const override;

    //! @copydoc request::user_agent()
    const char_t* user_agent(void) const override;

private:
    bool parse_method(int32_t& ch);
    bool parse_uri(int32_t& ch);
    bool parse_version(int32_t& ch);

    //! Parses a header utilizing the lexer member. Returns true, if a header
    //! could successfully get parsed. Returning false means, that the lexer has
    //! reached the end of the file. The in/out parameter ch is -1 in this case.
    bool parse_header(const mime_handler& handler, int32_t& ch);

    bool set_header(const mime_handler& handler, header_key key,
                    char_t* const key_string, const char_t* value_string,
                    size_t value_length);

    bool set_connection(const mime_handler& handler, char_t* key_string,
                        const char_t* value_string, size_t value_length);

    bool set_content_length(const mime_handler& handler, char_t* key_string,
                            const char_t* value_string, size_t value_length);

    bool set_content_md5(const mime_handler& handler, char_t* key_string,
                         const char_t* value_string, size_t value_length);

    bool set_content_type(const mime_handler& handler, char_t* key_string,
                          const char_t* value_string, size_t value_length);

    bool set_date(const mime_handler& handler, char_t* key_string,
                  const char_t* value_string, size_t value_length);

    bool set_expect(const mime_handler& handler, char_t* key_string,
                    const char_t* value_string, size_t value_length);

    bool set_from(const mime_handler& handler, char_t* key_string,
                  const char_t* value_string, size_t value_length);


    bool set_referer(const mime_handler& handler, char_t* key_string,
                     const char_t* value_string, size_t value_length);

    bool set_user_agent(const mime_handler& handler, char_t* key_string,
                        const char_t* value_string, size_t value_length);

    static bool is_whitespace(const int32_t ch);
    static bool is_newline(const int32_t ch);
    static bool is_key_value_separator(const int32_t ch);

    lexer lexer_;

    http_verb method_;
    uri path_uri_;
    http_version version_;

    size_t content_length_;
    const char_t* content_md5_;
    size_t content_md5_length_;
    mime content_type_;
    const void* content_;
    bool is_keep_alive_set_;
    epoch_time_t date_;
    http_expectation expect_;
    const char_t* from_;
    const char_t* referer_;
    const char_t* user_agent_;

    //! Do not use this in any other case than for storing header fields and
    //! query entries! It is designed for only one purpose: To let the string's
    //! content decide the order of the elements in the map. This map has to
    //! enforce, that all characters are null-terminated!
    struct enforced_null_terminated_less {
        bool operator()(const char_t* const lhs, const char_t* const rhs) const;
    };

    std::map<const char_t* const, const char_t*, enforced_null_terminated_less>
        header_fields_;
    std::map<const char_t* const, const char_t*, enforced_null_terminated_less>
        query_entries_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_MEMORY_ALLOCATING_REQUEST_HPP
