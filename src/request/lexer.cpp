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
    , raw_()
    , index_(0)
    , head_(0)
    , tail_(0)
{
}

int32_t lexer::get(void)
{
    // The rewritten data is always limited by the tail not by its size.
    int32_t result;
    if ((index_ < tail_) || (true == fetch_more_data())) {
        // Converting the character into an unsigned character will preserve the
        // bit representation and enables the implementation to reuse all
        // negative numbers as error values.
        result = static_cast<uint8_t>(raw_[index_++]);
    } else {
        result = -1;
    }
    return result;
}

size_t lexer::index(void) const
{
    return index_;
}

const char_t* lexer::data(const size_t idx) const
{
    const char_t* result;
    if (idx < tail_) {
        result = &(raw_[idx]);
    } else {
        result = nullptr;
    }
    return result;
}

char_t* lexer::data(const size_t idx)
{
    char_t* result;
    if (idx < tail_) {
        result = &(raw_[idx]);
    } else {
        result = nullptr;
    }
    return result;
}

bool lexer::fetch_more_data()
{
    assert(head_ == raw_.size());

    const size_t start_tail = tail_;

    // The last character is assumed to be 0 if no data was already
    // processed.
    char_t last = (0 == tail_) ? '\0' : raw_[tail_ - 1];

    // Loop will break, when either something parseable was received or the
    // connection got closed.
    while ((tail_ == start_tail) &&
           (true == connection_->receive(raw_, 4096))) {

        // At least one character could get evaluated, because
        // block_device_interface::receive returns true, when at least one byte
        // was read.
        do {
            const char_t ch = raw_[head_];

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
        } while (head_ < raw_.size());
    }

    return (start_tail < tail_);
}

void lexer::fetch_more_data_copy(const char_t ch, char_t& last)
{
    // In any case one character of the input stream gets consumed.
    head_++;

    if (ch == '\r') {
        raw_[tail_++] = '\n';
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
        raw_[tail_++] = ch;
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
        raw_[tail_ - 1] = ' ';
        last = ' ';
    }

    // Return to copying the stream.
    state_ = lexer_state::copy;
}

void lexer::fetch_more_data_reached_body(void)
{
    // Nothing to be done (except speed up the rewriting), when body
    // is reached.
    head_ = raw_.size();
}

} // namespace hutzn
