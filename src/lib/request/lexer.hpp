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

#ifndef LIBHUTZNOHMD_REQUEST_LEXER_HPP
#define LIBHUTZNOHMD_REQUEST_LEXER_HPP

#include "libhutznohmd/request.hpp"

namespace hutzn
{

//! @brief Helps to parse a HTTP request.
//!
//! This class provides functionality to prepare the HTTP header for the parser.
//! It normalizes the header therefore (replaces CR-LF with LF, CR with LF and
//! any character combination described as LWS by the HTTP standard with a
//! space). This is all done within fetch_header. It stores the header and
//! content data and gives access to them. Rewriting of the header data is
//! possible.
class lexer
{
public:
    //! @brief Constructs the lexer.
    //!
    //! Initializes all data structures.
    //! @param[in] connection Connection to use as data input.
    explicit lexer(const connection_ptr& connection);

    //! @brief Reads the complete header.
    //!
    //! Moves already read parts of the content to the content buffer. Call this
    //! method before using the lexer. Returns whether the header data was read
    //! and normalized successfully. This is no statement whether the header
    //! data is valid.
    //! @return True when reading was successful and false if not.
    bool fetch_header(void);

    //! @brief Reads the complete content from the connection.
    //!
    //! The length must be given to the function and the header must be fetched
    //! successfully first! Returns whether the content could be fetched
    //! completely. Returns also false, when the header was not fetched yet or
    //! when the fetching failed.
    //! @param[in] length Number of bytes to read from the connection.
    //! @return           True when reading was successful and false if not.
    bool fetch_content(const size_t length);

    //! @brief Returns the next token.
    //!
    //! Returns the next character in the header or -1 when reaching the end of
    //! the read data. Valid characters are represented in the range 0..255.
    //! @return The next token.
    int32_t get(void);

    //! @brief Returns the previous index.
    //!
    //! This is either std::numeric_limits<size_t>::max() in case there is no
    //! previous index or a number in the interval [0 .. tail-1).
    //! @return The previous index.
    size_t prev_index(void) const;

    //! @brief Returns the current index.
    //!
    //! This is a number in the interval [0 .. tail).
    //! @return Current index.
    size_t index(void) const;

    //! @brief Sets the current index.
    //!
    //! This is a number in the interval [0 .. tail].
    //! @param[in] idx Index to set.
    void set_index(const size_t idx);

    //! @brief Returns a constant pointer on the header beginning at offset idx.
    //!
    //! It returns NULL, when the offset is out of scope.
    //! @param[in] idx Index to read.
    //! @return        Pointer to the requested index.
    const char_t* header_data(const size_t idx) const;

    //! @brief Returns a pointer on the header beginning at offset idx.
    //!
    //! It returns a NULL, when the offset is out of scope.
    //! @param[in] idx Index to read.
    //! @return        Pointer to the requested index.
    char_t* header_data(const size_t idx);

    //! @brief Returns a constant pointer on the content.
    //!
    //! Is NULL, when fetch_content was not yet successfully called.
    //! @return Pointer to the start of the content.
    const char_t* content(void) const;

    //! @brief Returns the length of the fetched content.
    //!
    //! Is also 0, when fetch_content was not yet successfully called.
    //! @return Number of bytes in the content.
    size_t content_length(void) const;

private:
    //! @brief Steps the state machine of fetch_header one time.
    void fetch_header_step(size_t& tail, size_t& head, char_t& last);

    //! @brief Called by the fetch_header state machine when in state copy.
    void fetch_header_copy(size_t& tail, size_t& head, const char_t ch,
                           char_t& last);

    //! @brief Called by the fetch_header state machine when in state
    //! possible_cr_lf.
    void fetch_header_possible_cr_lf(size_t& head, const char_t ch,
                                     char_t& last);

    //! @brief Called by the fetch_header state machine when in state
    //! possible_lws.
    void fetch_header_possible_lws(size_t& tail, size_t& head, const char_t ch,
                                   char_t& last);

    //! @brief Called by the fetch_header state machine when in state
    //! reached_content.
    void fetch_header_reached_content(size_t& tail, size_t& head);

    /*! @brief Defines a state machine for a HTTP lexer.

    @startuml{lexer_state_machine.svg} "Lexer's state machine"
    [*] --> copy
    copy --> possible_cr_lf : current is CR
    copy --> reached_content : last and current are LF
    copy --> possible_lws : current is LF and last not
    copy --> copy : else
    copy --> error : receive returns error
    possible_cr_lf --> reached_content : last is LF
    possible_cr_lf --> possible_lws : last is not LF
    possible_cr_lf --> error : receive returns error
    possible_lws --> copy
    possible_lws --> error : receive returns error
    error --> [*]
    reached_content --> [*]
    @enduml
    */
    enum class lexer_state {
        //! Copies one character from head to tail.
        copy = 0,

        //! Tries to find a CR-LF sequence. This gets converted to LF.
        possible_cr_lf = 1,

        //! Tries to find a LWS sequence. This gets converted to a space.
        possible_lws = 2,

        //! Final state when two LFs are at the tail.
        reached_content = 3,

        //! Receive could return an error. Then this state is set.
        error = 4
    };

    //! Connection to read data from.
    connection_ptr connection_;

    //! Current state of the lexer.
    lexer_state state_;

    //! Contains the header data. This data is partially reused as storage to
    //! save heap space and allocation time.
    buffer header_;

    //! Contains the content data.
    buffer content_;

    //! True when the fetch finished successfully.
    bool fetch_content_succeeded_;

    //! Current index.
    size_t index_;
};

} // namespace hutzn

#endif // LIBHUTZNOHMD_REQUEST_LEXER_HPP
