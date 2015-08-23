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

#include <cassert>

#include "lexer.hpp"

namespace hutzn
{

lexer::lexer(const connection_pointer& connection)
    : connection_(connection)
    , state_(lexer_state::copy)
    , header_()
    , content_()
    , index_(0)
    , head_(0)
    , tail_(0)
{
}

bool lexer::fetch_header(void)
{
    // Fetching the whole header is done, when two consecutive newline
    // characters are found. Fetching and reencoding more and more data is
    // necessary to reach this.
    bool search_is_finished = (header_.size() == 0);
    size_t i = 0;
    size_t number_of_consecutive_newlines = 0;

    // Loops until either two consecutive newlines are found or the end of the
    // data is reached.
    while (false == search_is_finished) {
        if ((i < tail_) || (true == fetch_more_data())) {
            // Increase the counter in case of a newline and otherwise reset it
            // to 0.
            if ('\n' == header_[i]) {
                number_of_consecutive_newlines++;
            } else {
                number_of_consecutive_newlines = 0;
            }

            // Updating the finished flag..
            search_is_finished = (number_of_consecutive_newlines >= 2);

            // Increasing index not in the else-branch, because in the
            // else-branch the end is already found.
            i++;
        } else {
            // Fetching was necessary but failed. Don't know where to get more
            // data.
            search_is_finished = true;
        }
    }

    // Either the header's or the stream's end was found. While fetch_more_data
    // is resizing the header to its real size after finding the the header's
    // end the stream's end is found in both cases.
    assert(i == header_.size());

    return (number_of_consecutive_newlines >= 2);
}

int32_t lexer::get(void)
{
    // The rewritten data is always limited by the tail not by its size.
    int32_t result;
    if ((index_ < tail_) || (true == fetch_more_data())) {
        // Converting the character into an unsigned character will preserve the
        // bit representation and enables the implementation to reuse all
        // negative numbers as error values.
        result = static_cast<uint8_t>(header_[index_++]);
    } else {
        result = -1;
    }
    return result;
}

size_t lexer::prev_index(void) const
{
    return index_ - 1;
}

size_t lexer::index(void) const
{
    return index_;
}

void lexer::set_index(const size_t idx)
{
    if (idx <= tail_) {
        index_ = idx;
    }
}

const char_t* lexer::header_data(const size_t idx) const
{
    const char_t* result;
    if (idx < tail_) {
        result = &(header_[idx]);
    } else {
        result = nullptr;
    }
    return result;
}

char_t* lexer::header_data(const size_t idx)
{
    char_t* result;
    if (idx < tail_) {
        result = &(header_[idx]);
    } else {
        result = nullptr;
    }
    return result;
}

bool lexer::fetch_more_data()
{
    constexpr size_t chunk_size = 4000;

    assert(head_ == header_.size());

    const size_t start_tail = tail_;

    // The last character is assumed to be 0 if no data was already
    // processed.
    char_t last = (0 == tail_) ? '\0' : header_[tail_ - 1];

    // Loop will break, when either something parseable was received or the
    // connection got closed.
    while ((tail_ == start_tail) &&
           (true == connection_->receive(header_, chunk_size))) {

        // At least one character could get evaluated, because
        // block_device_interface::receive returns true, when at least one byte
        // was read.
        do {
            const char_t ch = header_[head_];

            switch (state_) {
            case lexer_state::copy:
                fetch_more_data_copy(ch, last);
                break;

            case lexer_state::possible_cr_lf:
                fetch_more_data_possible_cr_lf(ch, last);
                break;

            case lexer_state::possible_lws:
                fetch_more_data_possible_lws(ch, last);
                break;

            case lexer_state::reached_body:
                fetch_more_data_reached_body();
                break;

            default:
                // Never reach an undefined lexer state.
                assert(false);
                break;
            }
        } while (head_ < header_.size());
    }

    return (start_tail < tail_);
}

void lexer::fetch_more_data_copy(const char_t ch, char_t& last)
{
    // In any case one character of the input stream gets consumed.
    head_++;

    if (ch == '\r') {
        header_[tail_++] = '\n';
        // Delay updating the last character, because the last
        // character is necessary in the next state to determine
        // transition into lexer_state::possible_cr_lf.
        state_ = lexer_state::possible_cr_lf;
    } else {
        // If the current is a newline and the last was no newline,
        // then check for a possible lws token.
        if (ch == '\n') {
            if (last == '\n') {
                state_ = lexer_state::reached_body;
            } else {
                state_ = lexer_state::possible_lws;
            }
        }

        // Update the last character here, because it is necessary
        // to determine the next state. Copy also the character.
        header_[tail_++] = ch;
        last = ch;
    }
}

void lexer::fetch_more_data_possible_cr_lf(const char_t ch, char_t& last)
{
    // Eat up one newline if available, because earlier there was a
    // carriage return and cr-lf will get to one newline.
    if (ch == '\n') {
        head_++;
    }

    // The current character is already a newline. The last
    // character was not updated when the carriage return got
    // parsed.
    if (last == '\n') {
        // We are reaching the body when two newlines are getting
        // parsed. The last character does not get updated, because
        // it is already newline.
        state_ = lexer_state::reached_body;
    } else {
        state_ = lexer_state::possible_lws;

        // Updating the last character, which was delayed.
        last = '\n';
    }
}

void lexer::fetch_more_data_possible_lws(const char_t ch, char_t& last)
{
    // The parser does reach this state, when the character before
    // was a newline. There exists a LWS token, when the current
    // character is either space or tab.
    if ((ch == ' ') || (ch == '\t')) {
        // Consume this character.
        head_++;

        // A LWS token overrules the read character with a space.
        // The last character (newline) was already written and gets
        // therefore overwritten.
        assert(tail_ > 0);
        header_[tail_ - 1] = ' ';
        last = ' ';
    }

    // Return to copying the stream.
    state_ = lexer_state::copy;
}

void lexer::fetch_more_data_reached_body(void)
{
    // Found the content. Moving the remaining header data into the content
    // data.
    content_.insert(content_.end(), header_.begin() + head_, header_.end());
    header_.resize(head_, '\0');

    // Nothing more to be done, when body is reached. The loop will break
    // afterwards, because the end of the header is reached.
}

} // namespace hutzn
