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

#include <libhutznohmd/request.hpp>

#include <request/lexer.hpp>
#include <utility/trie.hpp>

namespace hutzn
{

class request : public request_interface
{
public:
    explicit request(const connection_pointer& connection);

    bool parse(void);

    bool parse_method(int32_t& ch);
    bool parse_uri(int32_t& ch);
    bool parse_version(int32_t& ch);

    //! Parses a header utilizing the lexer member. Returns true, if a header
    //! could successfully get parsed. Returning false means, that the lexer has
    //! reached the end of the file. The in/out parameter c is -1 in this case.
    bool parse_header(int32_t& ch);

    http_verb method(void) const override;
    const char_t* path(void) const override;
    const char_t* host(void) const override;
    const char_t* query(const char_t* const key) const override;
    const char_t* fragment(void) const override;
    http_version version(void) const override;
    const char_t* header_value(const char_t* const name) const override;
    bool keeps_connection(void) const override;
    time_t date(void) const override;
    void* content(void) const override;
    size_t content_length(void) const override;
    mime content_type(void) const override;
    bool accept(void*& handle, mime& type) const override;
    http_expectation expect(void) const override;
    const char_t* from(void) const override;
    const char_t* referer(void) const override;
    const char_t* user_agent(void) const override;

private:
    static bool is_whitespace(const int32_t ch);
    static bool is_newline(const int32_t ch);
    static bool is_key_value_seperator(const int32_t ch);

    lexer lexer_;

    size_t maximum_method_length_;
    trie<http_verb> methods_;

    size_t maximum_version_length_;
    trie<http_version> versions_;

    http_verb method_;
    http_version version_;

    std::map<const char_t* const, const char_t*> header_fields_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_REQUEST_HPP
