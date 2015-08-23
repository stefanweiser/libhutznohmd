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

#include <libhutznohmd/request.hpp>

namespace hutzn
{

class lexer
{
public:
    explicit lexer(const connection_pointer& connection);

    //! Returns the next character in the stream or -1 when reaching the end of
    //! the file. Valid characters are represented in the range 0..255.
    int32_t get(void);

    //! Returns the current index.
    size_t index(void) const;

    //! Returns a constant pointer on the data stream beginning at offset idx.
    const char_t* data(const size_t idx) const;

    //! Returns a pointer on the data stream beginning at offset idx.
    char_t* data(const size_t idx);

private:
    bool fetch_more_data();

    void fetch_more_data_copy(const char_t ch, char_t& last);
    void fetch_more_data_possible_cr_lf(const char_t ch, char_t& last);
    void fetch_more_data_possible_lws(const char_t ch, char_t& last);
    void fetch_more_data_reached_body(void);

    enum class lexer_state {
        copy = 0,
        possible_cr_lf = 1,
        possible_lws = 2,
        reached_body = 3
    };

    connection_pointer connection_;
    lexer_state state_;
    buffer raw_;
    size_t index_;
    size_t head_;
    size_t tail_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_LEXER_HPP
