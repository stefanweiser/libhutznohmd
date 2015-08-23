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

class lexer
{
public:
    explicit lexer(const connection_pointer& connection);

    //! Reads the complete header and possible already read parts of the content
    //! is moved to the content buffer. Call this method before using the lexer.
    //! Returns whether the header was read successfully.
    bool fetch_header(void);

    //! Returns the next character in the header or -1 when reaching the end of
    //! the read data. Valid characters are represented in the range 0..255.
    int32_t get(void);

    //! Returns the previous index. This is a number between or equal to
    //! std::numeric_limits<size_t>;;max() and tail minus 2.
    size_t prev_index(void) const;

    //! Returns the current index. This is a number in the interval [0 .. tail).
    size_t index(void) const;

    //! Sets the current index. This is a number in the interval [0 .. tail].
    void set_index(const size_t idx);

    //! Returns a constant pointer on the header beginning at offset idx.
    const char_t* header_data(const size_t idx) const;

    //! Returns a pointer on the header beginning at offset idx.
    char_t* header_data(const size_t idx);

private:
    void fetch_more_data_copy(const char_t ch, char_t& last);
    void fetch_more_data_possible_cr_lf(const char_t ch, char_t& last);
    void fetch_more_data_possible_lws(const char_t ch, char_t& last);
    void fetch_more_data_reached_body(void);

    enum class lexer_state {
        copy = 0,
        possible_cr_lf = 1,
        possible_lws = 2,
        reached_body = 3,
        error = 4
    };

    connection_pointer connection_;
    lexer_state state_;
    buffer header_;
    buffer content_;
    size_t index_;
    size_t head_;
    size_t tail_;
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
