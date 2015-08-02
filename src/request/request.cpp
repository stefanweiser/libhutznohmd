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

#include "request.hpp"

namespace hutzn
{

request::request(const connection_pointer& connection)
    : connection_(connection)
    , state_(request_parser_state::init)
    , raw_()
    , head_(0)
    , tail_(0)
    , last_char_(0)
{
}

bool request::fetch_header()
{
    bool result = ((request_parser_state::fetching_body == state_) ||
                   (request_parser_state::success == state_));

    // Fetch the header only if the state is init.
    if (state_ == request_parser_state::init) {
        bool finished_fetching = false;
        while (false == finished_fetching) {
            finished_fetching = (get() == -1);
        }
    }
    return result;
}

http_verb request::method(void) const
{
    return http_verb::GET;
}

const char_t* request::path(void) const
{
    return nullptr;
}

const char_t* request::host(void) const
{
    return nullptr;
}

const char_t* request::query(const char_t* const /*key*/) const
{
    return nullptr;
}

const char_t* request::fragment(void) const
{
    return nullptr;
}

http_version request::version(void) const
{
    return http_version::HTTP_UNKNOWN;
}

const char_t* request::header_value(const char_t* const /*name*/) const
{
    return nullptr;
}

bool request::keeps_connection(void) const
{
    return false;
}

time_t request::date(void) const
{
    return 0;
}

void* request::content(void) const
{
    return nullptr;
}

size_t request::content_length(void) const
{
    return 0;
}

mime request::content_type(void) const
{
    return mime(mime_type::INVALID, mime_subtype::INVALID);
}

bool request::accept(void*& /*handle*/, mime& /*type*/) const
{
    return false;
}

http_expectation request::expect(void) const
{
    return http_expectation::UNKNOWN;
}

const char_t* request::from(void) const
{
    return nullptr;
}

const char_t* request::referer(void) const
{
    return nullptr;
}

const char_t* request::user_agent(void) const
{
    return nullptr;
}

bool request::probably_fetch_more_data()
{
    if ((head_ >= raw_.size()) && (false == connection_->receive(raw_, 4096))) {
        return false;
    }
    return true;
}

int32_t request::get_character()
{
    if (false == probably_fetch_more_data()) {
        return -1;
    }
    return static_cast<uint8_t>(raw_[head_++]);
}

int32_t request::peek_character()
{
    if (false == probably_fetch_more_data()) {
        return -1;
    }
    return static_cast<uint8_t>(raw_[head_]);
}

int32_t request::get()
{
    int32_t result = get_character();
    if (result == '\r') {
        if (peek_character() == '\n') {
            get_character();
        }
        result = '\n';
    }

    if ((result == '\n') && (last_char_ != '\n')) {
        const int32_t n = peek_character();
        if ((n == ' ') || (n == '\t')) {
            get_character();
            result = ' ';
        }
    }

    last_char_ = static_cast<char_t>(result);

    if (result != -1) {
        raw_[tail_++] = last_char_;
    }

    return result;
}

} // namespace hutzn
