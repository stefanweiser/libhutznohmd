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

#include "lexer.hpp"

#include <cassert>

namespace hutzn
{

lexer::lexer(const connection_pointer& connection)
    : connection_(connection)
    , state_(lexer_state::copy)
    , header_()
    , content_()
    , fetch_content_succeeded_(false)
    , index_(0)
{
}

bool lexer::fetch_header(void)
{
    size_t tail = 0;
    size_t head = 0;

    // The last character is assumed to be 0 if no data was already processed.
    char_t last = '\0';

    static const size_t chunk_size = 4000;

    // Loop will break, when one of the end states are reached. This will also
    // guard calling the method twice or more.
    while ((state_ != lexer_state::reached_content) &&
           (state_ != lexer_state::error)) {

        // Need more data.
        if (connection_->receive(header_, chunk_size)) {

            // At least one character is available to get evaluated, because
            // block_device_interface::receive returns true, when at least one
            // byte was read.
            do {
                fetch_header_step(tail, head, last);
            } while (head < header_.size());
        } else {
            state_ = lexer_state::error;
        }
    }

    // Cutting off the header may be already done during
    // fetch_header_reached_body, but when the header is received as a whole
    // chunk this method will not get called. Therefore the resize has to be
    // repeated here, because nonetheless
    if (tail > 0) {
        assert(tail <= header_.size());
        header_.resize(tail);
    }

    // After the loop, the state has to be one of the end states. The method
    // returns true, when the loop reached the body and therefore the header is
    // complete.
    assert((state_ == lexer_state::reached_content) ||
           (state_ == lexer_state::error));
    return state_ == lexer_state::reached_content;
}

bool lexer::fetch_content(const size_t length)
{
    bool result = false;
    if (state_ == lexer_state::reached_content) {
        // Never fetch more data than necessary.
        assert(content_.size() <= length);

        // Fetching more data when necessary.
        bool fetch_more = (content_.size() < length);
        while (fetch_more) {

            // This must be done in a loop, because receive returns true, if
            // something is read. There is no gurantee, that all the necessary
            // bytes are read.
            const size_t bytes_to_read = length - content_.size();
            if (connection_->receive(content_, bytes_to_read)) {
                // Recalculate fetch_more. Continue receiving, when the content
                // is not yet complete.
                fetch_more = (content_.size() < length);
            } else {
                // Stop fetching, when receive fails.
                fetch_more = false;
            }
        }

        // Returns true, when enough data is available.
        result = (length == content_.size());

        // Remember, that fetch_content once returned true.
        fetch_content_succeeded_ = result;
    }

    return result;
}

int32_t lexer::get(void)
{
    int32_t result;
    if (index_ < header_.size()) {
        // Converting the character into an unsigned character first will
        // preserve the bit representation and enables the implementation to
        // reuse all negative numbers as error values.
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
    // Set new index only when in range.
    if (idx <= header_.size()) {
        index_ = idx;
    }
}

const char_t* lexer::header_data(const size_t idx) const
{
    const char_t* result;
    if (idx < header_.size()) {
        result = &(header_[idx]);
    } else {
        result = NULL;
    }
    return result;
}

char_t* lexer::header_data(const size_t idx)
{
    char_t* result;
    if (idx < header_.size()) {
        result = &(header_[idx]);
    } else {
        result = NULL;
    }
    return result;
}

const char_t* lexer::content(void) const
{
    const char_t* result;
    if (fetch_content_succeeded_) {
        result = &(content_[0]);
    } else {
        result = NULL;
    }
    return result;
}

size_t lexer::content_length(void) const
{
    size_t result;
    if (fetch_content_succeeded_) {
        result = content_.size();
    } else {
        result = 0;
    }
    return result;
}

void lexer::fetch_header_step(size_t& tail, size_t& head, char_t& last)
{
    const char_t ch = header_[head];

    switch (state_) {
    case lexer_state::copy:
        fetch_header_copy(tail, head, ch, last);
        break;

    case lexer_state::possible_cr_lf:
        fetch_header_possible_cr_lf(head, ch, last);
        break;

    case lexer_state::possible_lws:
        fetch_header_possible_lws(tail, head, ch, last);
        break;

    case lexer_state::reached_content:
        fetch_header_reached_content(tail, head);
        break;

    // Also treat errors as reason to crash, because the only way to
    // get in error state is when receive returns false. The program
    // will not get here in this case, because this is a reason to
    // break from the outermost while loop first.
    case lexer_state::error:
    default:
        assert(false);
        break;
    }
}

void lexer::fetch_header_copy(size_t& tail, size_t& head, const char_t ch,
                              char_t& last)
{
    // In any case one character of the input stream gets consumed.
    head++;

    if (ch == '\r') {
        header_[tail++] = '\n';
        // Delay updating the last character, because the last
        // character is necessary in the next state to determine
        // transition into lexer_state::possible_cr_lf.
        state_ = lexer_state::possible_cr_lf;
    } else {
        // If the current is a newline and the last was no newline,
        // then check for a possible lws token.
        if (ch == '\n') {
            if (last == '\n') {
                state_ = lexer_state::reached_content;
            } else {
                state_ = lexer_state::possible_lws;
            }
        }

        // Update the last character here, because it is necessary
        // to determine the next state. Copy also the character.
        header_[tail++] = ch;
        last = ch;
    }
}

void lexer::fetch_header_possible_cr_lf(size_t& head, const char_t ch,
                                        char_t& last)
{
    // Eat up one newline if available, because earlier there was a
    // carriage return and cr-lf will get to one newline.
    if (ch == '\n') {
        head++;
    }

    // The current character is already a newline. The last
    // character was not updated when the carriage return got
    // parsed.
    if (last == '\n') {
        // We are reaching the body when two newlines are getting
        // parsed. The last character does not get updated, because
        // it is already newline.
        state_ = lexer_state::reached_content;
    } else {
        state_ = lexer_state::possible_lws;

        // Updating the last character, which was delayed.
        last = '\n';
    }
}

void lexer::fetch_header_possible_lws(size_t& tail, size_t& head,
                                      const char_t ch, char_t& last)
{
    // The parser does reach this state, when the character before
    // was a newline. There exists a LWS token, when the current
    // character is either space or tab.
    if ((ch == ' ') || (ch == '\t')) {
        // Consume this character.
        head++;

        // A LWS token overrules the read character with a space.
        // The last character (newline) was already written and gets
        // therefore overwritten.
        assert(tail > 0);
        header_[tail - 1] = ' ';
        last = ' ';
    }

    // Return to copying the stream.
    state_ = lexer_state::copy;
}

void lexer::fetch_header_reached_content(size_t& tail, size_t& head)
{
    // Found the content. Moving the remaining header data into the content
    // data.
    content_.insert(content_.end(),
                    header_.begin() + static_cast<ssize_t>(head),
                    header_.end());
    assert(tail <= header_.size());
    header_.resize(tail);

    // Nothing more to be done, when body is reached. The loop will break
    // afterwards, because the end of the header is reached.
}

} // namespace hutzn
