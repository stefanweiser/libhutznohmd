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

#include <cmath>
#include <cstdint>
#include <limits>
#include <map>

#include <http/parser/utility/timestamp.hpp>
#include <http/parser/utility/trie.hpp>

#include "request_parser.hpp"

namespace hutzn
{

namespace http
{

request_parser::request_parser(const anonymous_int_function& get_functor,
                               const anonymous_int_function& peek_functor)
    : common_(get_functor, peek_functor)
    , method_(hutzn::method::UNKNOWN)
    , request_uri_()
    , from_uri_()
    , accept_header_()
{
}

void request_parser::parse()
{
    if (parser_state::UNFINISHED != common_.state_) {
        return;
    }

    int32_t character = common_.lexer_.get_non_whitespace();
    {
        using value_info = trie<hutzn::method>::value_info;
        static const std::vector<value_info> types = {
            {value_info{"head", hutzn::method::HEAD},
             value_info{"get", hutzn::method::GET},
             value_info{"put", hutzn::method::PUT},
             value_info{"delete", hutzn::method::DELETE},
             value_info{"post", hutzn::method::POST},
             value_info{"trace", hutzn::method::TRACE},
             value_info{"options", hutzn::method::OPTIONS},
             value_info{"connect", hutzn::method::CONNECT}}};

        static const trie<hutzn::method> t(types, hutzn::method::UNKNOWN);
        push_back_string<32> tmp;
        method_ = t.parse(character, tmp, common_.lexer_);
    }

    if (hutzn::method::UNKNOWN == method_) {
        common_.state_ = parser_state::ERROR;
        return;
    }

    character = common_.lexer_.get_non_whitespace();
    if (!request_uri_.parse(common_.lexer_, character, false)) {
        common_.state_ = parser_state::ERROR;
        return;
    }

    // A request uri is everytime HTTP.
    request_uri_.set_scheme(uri_scheme::HTTP);

    character = common_.lexer_.get_non_whitespace();
    {
        using value_type = std::tuple<http::version, connection_type>;
        using value_info = trie<value_type>::value_info;
        static const std::vector<value_info> types = {
            {value_info{"http/1.0", value_type{http::version::HTTP_1_0,
                                               connection_type::CLOSE}},
             value_info{"http/1.1", value_type{http::version::HTTP_1_1,
                                               connection_type::KEEP_ALIVE}}}};

        static const trie<value_type> t(
            types,
            value_type{http::version::HTTP_UNKNOWN, connection_type::ERROR});
        push_back_string<32> tmp;
        std::tie(common_.version_, common_.connection_) =
            t.parse(character, tmp, common_.lexer_);
    }

    if ((http::version::HTTP_UNKNOWN == common_.version_) ||
        (character != '\n')) {
        common_.state_ = parser_state::ERROR;
        return;
    }

    character = common_.lexer_.get();
    if (!parse_headers(character)) {
        common_.state_ = parser_state::ERROR;
        return;
    }
    common_.state_ = parser_state::SUCCEEDED;
}

bool request_parser::valid() const
{
    return parser_state::SUCCEEDED == common_.state_;
}

const http::version& request_parser::version() const
{
    return common_.version_;
}

const std::map<std::string, std::string>& request_parser::headers() const
{
    return common_.headers_;
}

const size_t& request_parser::content_length() const
{
    return common_.content_length_;
}

const media_type_interface& request_parser::content_type() const
{
    return common_.content_type_;
}

const time_t& request_parser::date() const
{
    return common_.date_;
}

bool request_parser::keeps_connection() const
{
    return common_.connection_ == connection_type::KEEP_ALIVE;
}

const std::array<uint8_t, 16>& request_parser::md5() const
{
    return common_.md5_;
}

bool request_parser::has_md5() const
{
    return common_.has_md5_;
}

const hutzn::method& request_parser::method() const
{
    return method_;
}

const uri_interface& request_parser::request_uri() const
{
    return request_uri_;
}

const uri_interface& request_parser::from_uri() const
{
    return from_uri_;
}

std::vector<const media_type_interface*> request_parser::accept_header() const
{
    std::vector<const media_type_interface*> result;
    for (size_t i = 0; i < accept_header_.size(); i++) {
        result.push_back(&(accept_header_[i]));
    }
    return result;
}

bool request_parser::parse_accept(int32_t& character)
{
    do {
        accept_header_.push_back(media_type());
        if (!accept_header_.back().parse(common_.lexer_, character)) {
            return false;
        }

        if (',' == character) {
            character = common_.lexer_.get_non_whitespace();
        }
    } while (character != '\n');

    return true;
}

bool request_parser::parse_connection(int32_t& character)
{
    return common_.parse_connection(character);
}

bool request_parser::parse_content_length(int32_t& character)
{
    return common_.parse_content_length(character);
}

bool request_parser::parse_content_md5(int32_t& character)
{
    return common_.parse_content_md5(character);
}

bool request_parser::parse_content_type(int32_t& character)
{
    return common_.parse_content_type(character);
}

bool request_parser::parse_date(int32_t& character)
{
    return common_.parse_date(character);
}

bool request_parser::parse_from(int32_t& character)
{
    from_uri_.set_scheme(uri_scheme::MAILTO);
    if (!from_uri_.parse(common_.lexer_, character, true)) {
        return false;
    }

    return true;
}

bool request_parser::parse_host(int32_t& character)
{
    uri host;
    if (!host.parse(common_.lexer_, character, true)) {
        return false;
    }

    // This URI is a HTTP URI.
    if (0 == host.port()) {
        host.set_port(80);
    }

    // Simply overwrite the scheme and the authority of the request URI.
    request_uri_.set_scheme(uri_scheme::HTTP);
    request_uri_.set_userinfo(host.userinfo());
    request_uri_.set_host(host.host());
    request_uri_.set_port(host.port());

    return true;
}

bool request_parser::parse_headers(int32_t& character)
{
    using trie_value_ = base_parser::trie_value<request_parser>;
    using value_info = trie<trie_value_>::value_info;
    static const std::vector<value_info> types = {
        {value_info{"content-length",
                    trie_value_{&request_parser::parse_content_length, 0}},
         value_info{"content-md5",
                    trie_value_{&request_parser::parse_content_md5, 1}},
         value_info{"content-type",
                    trie_value_{&request_parser::parse_content_type, 2}},
         value_info{"date", trie_value_{&request_parser::parse_date, 3}},
         value_info{"connection",
                    trie_value_{&request_parser::parse_connection, 4}},
         value_info{"accept", trie_value_{&request_parser::parse_accept, 5}},
         value_info{"host", trie_value_{&request_parser::parse_host, 6}},
         value_info{"from", trie_value_{&request_parser::parse_from, 7}}}};

    while (character != '\n') {
        if (!common_.parse_generic_header(types, *this, character)) {
            return false;
        }
        character = common_.lexer_.get();
    }
    return true;
}

} // namespace http

} // namespace hutzn
