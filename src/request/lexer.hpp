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

#ifndef LIBHUTZNOHMD_REQUEST_LEXER_HPP
#define LIBHUTZNOHMD_REQUEST_LEXER_HPP

#include <cassert>

#include <libhutznohmd/request.hpp>

namespace hutzn
{

//! The library's lexer provides functionality to prepare the HTTP header for
//! the parser. It normalizes the header therefore (replaces \r\n with \n, \r
//! with \n and any character combination described as LWS by the HTTP standard
//! with a space). This is all done within fetch_header. It stores the header
//! and content data and gives access to them. Rewriting of the header data is
//! possible.
class lexer
{
public:
    explicit lexer(const connection_pointer& connection);

    //! Reads the complete header and moves already read parts of the content
    //! to the content buffer. Call this method before using the lexer. Returns
    //! whether the header data was read and normalized successfully. This is no
    //! statement whether the header data is valid.
    bool fetch_header(void);

    //! Reads the complete content from the connection. The length must be given
    //! to the function and the header must be fetched first! Returns whether
    //! the content could be fetched completely. Returns also false, when the
    //! header was not fetched yet.
    bool fetch_content(const size_t content_length);

    //! Returns the next character in the header or -1 when reaching the end of
    //! the read data. Valid characters are represented in the range 0..255.
    int32_t get(void);

    //! Returns the previous index. This is either
    //! std::numeric_limits<size_t>::max() in case there is no previous index or
    //! a number in the interval [0 .. tail-1).
    size_t prev_index(void) const;

    //! Returns the current index. This is a number in the interval [0 .. tail).
    size_t index(void) const;

    //! Sets the current index. This is a number in the interval [0 .. tail].
    void set_index(const size_t idx);

    //! Returns a constant pointer on the header beginning at offset idx.
    const char_t* header_data(const size_t idx) const;

    //! Returns a pointer on the header beginning at offset idx.
    char_t* header_data(const size_t idx);

    //! Returns a constant pointer on the content.
    const char_t* content(void) const;

private:
    //! Called by the fetch_header state machine when in state copy.
    void fetch_header_copy(size_t& tail, size_t& head, const char_t ch,
                           char_t& last);

    //! Called by the fetch_header state machine when in state possible_cr_lf.
    void fetch_header_possible_cr_lf(size_t& head, const char_t ch,
                                     char_t& last);

    //! Called by the fetch_header state machine when in state possible_lws.
    void fetch_header_possible_lws(size_t& tail, size_t& head, const char_t ch,
                                   char_t& last);

    //! Called by the fetch_header state machine when in state reached_content.
    void fetch_header_reached_content(size_t& tail, size_t& head);

    enum class lexer_state {
        copy = 0,
        possible_cr_lf = 1,
        possible_lws = 2,
        reached_content = 3,
        error = 4
    };

    connection_pointer connection_;
    lexer_state state_;
    buffer header_;
    buffer content_;
    size_t index_;
};

//! Parses a token and returns the number of characters read from the lexer. If
//! no valid stop character is found, the index gets resetted to its original
//! value and 0 is returned from the function.
template <typename stop_function>
size_t parse_specific(lexer& lex, int32_t& ch, const stop_function& stop)
{
    const size_t begin_index = lex.index();
    assert(begin_index > 0);
    size_t parsed_characters = 0;
    int32_t c = ch;
    while (c != -1) {
        if (true == stop(static_cast<uint8_t>(c))) {
            parsed_characters = lex.index() - begin_index;
            ch = c;
            c = -1;
        } else {
            c = lex.get();
        }
    }

    if (0 == parsed_characters) {
        lex.set_index(begin_index - 1);
        ch = lex.get();
    }

    return parsed_characters;
}

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_LEXER_HPP
