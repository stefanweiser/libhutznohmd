/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <cstdint>
#include <limits>
#include <map>

#include <http/parser/utility/date.hpp>
#include <http/parser/utility/trie.hpp>

#include "requestparser.hpp"

namespace rest
{

namespace http
{

request_parser::request_parser(const anonymous_int_function & get_functor,
                               const anonymous_int_function & peek_functor)
    : base_parser(get_functor, peek_functor)
    , method_(rest::http::method::UNKNOWN)
    , request_uri_()
    , accept_header_()
{}

void request_parser::parse()
{
    if (parser_state::UNFINISHED != state_) {
        return;
    }

    int32_t character = lexer_.get_non_whitespace();
    {
        using value_info = trie<rest::http::method>::value_info;
        static const std::vector<value_info> types = {{
                value_info{"head", rest::http::method::HEAD},
                value_info{"get", rest::http::method::GET},
                value_info{"put", rest::http::method::PUT},
                value_info{"delete", rest::http::method::DELETE},
                value_info{"post", rest::http::method::POST},
                value_info{"trace", rest::http::method::TRACE},
                value_info{"options", rest::http::method::OPTIONS},
                value_info{"connect", rest::http::method::CONNECT}
            }
        };

        static const trie<rest::http::method> t(types, rest::http::method::UNKNOWN);
        push_back_string<32> tmp;
        method_ = t.parse(character, tmp, lexer_);
    }

    if (rest::http::method::UNKNOWN == method_) {
        state_ = parser_state::ERROR;
        return;
    }

    character = lexer_.get_non_whitespace();
    if (false == request_uri_.parse(lexer_, character)) {
        state_ = parser_state::ERROR;
        return;
    }

    character = lexer_.get_non_whitespace();
    {
        using value_type = std::tuple<rest::http::version, connection_type>;
        using value_info = trie<value_type>::value_info;
        static const std::vector<value_info> types = {{
                value_info{"http/1.0", value_type{
                        rest::http::version::HTTP_1_0,
                        connection_type::CLOSE
                    }
                },
                value_info{"http/1.1", value_type{
                        rest::http::version::HTTP_1_1,
                        connection_type::KEEP_ALIVE
                    }
                }
            }
        };

        static const trie<value_type> t(types, value_type {
            rest::http::version::HTTP_UNKNOWN,
            connection_type::ERROR
        });
        push_back_string<32> tmp;
        std::tie(version_, connection_) = t.parse(character, tmp, lexer_);
    }

    if ((rest::http::version::HTTP_UNKNOWN == version_) ||
        (character != '\n')) {
        state_ = parser_state::ERROR;
        return;
    }

    character = lexer_.get();
    if (false == parse_headers(character)) {
        state_ = parser_state::ERROR;
        return;
    }
    state_ = parser_state::SUCCEEDED;
}

const rest::http::method & request_parser::method() const
{
    return method_;
}

const uri_interface & request_parser::request_uri() const
{
    return request_uri_;
}

bool request_parser::parse_accept(int32_t & character)
{
    do {
        accept_header_.push_back(std::unique_ptr<media_type>(new media_type(lexer_)));
        if (false == accept_header_.back()->parse(character)) {
            return false;
        }

        if (',' == character) {
            character = lexer_.get_non_whitespace();
        }
    } while (character != '\n');

    return true;
}

bool request_parser::parse_headers(int32_t & character)
{
    using trie_value_ = trie_value<request_parser>;
    using value_info = trie<trie_value_>::value_info;
    static const std::vector<value_info> types = {{
            value_info{"content-length", trie_value_{&request_parser::parse_content_length, 0}},
            value_info{"content-md5", trie_value_{&request_parser::parse_content_md5, 1}},
            value_info{"content-type", trie_value_{&request_parser::parse_content_type, 2}},
            value_info{"date", trie_value_{&request_parser::parse_date, 3}},
            value_info{"connection", trie_value_{&request_parser::parse_connection, 4}},
            value_info{"accept", trie_value_{&request_parser::parse_accept, 5}}
        }
    };

    while (character != '\n') {
        if (false == parse_generic_header<request_parser>(types, character)) {
            return false;
        }
        character = lexer_.get();
    }
    return true;
}

} // namespace http

} // namespace rest
