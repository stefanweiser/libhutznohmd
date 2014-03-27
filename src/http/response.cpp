/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <cctype>
#include <iterator>
#include <set>
#include <sstream>

#include "response.hpp"

namespace rest
{

namespace http
{

response::response(const rest::socket::connection_pointer & connection)
    : connection_(connection)
    , status_code_(status_code::INTERNAL_SERVER_ERROR)
    , version_(version::HTTP_1_1)
    , headers_()
    , data_()
{}

void response::deliver()
{
    std::stringstream head;
    deliver_version(head, version_);
    deliver_status_code_text(head, status_code_);
    head << "\r\n";

    for (const auto & pair : headers_) {
        head << pair.first << ": " << pair.second << "\r\n";
    }
    head << "\r\n";

    connection_->send(head.str());
    connection_->send(data_);
}

void response::set_status_code(const rest::http::status_code & status_code)
{
    status_code_ = status_code;
}

void response::set_version(const rest::http::version & version)
{
    version_ = version;
}

void response::set_header(const std::string & key, const std::string & value)
{
    headers_[key] = value;
}

void response::set_data(const rest::buffer & data)
{
    data_ = data;
}

void response::deliver_version(std::ostream & os, const rest::http::version & version)
{
    if (version == rest::http::version::HTTP_1_0) {
        os << "HTTP/1.0 ";
    } else if (version == rest::http::version::HTTP_1_1) {
        os << "HTTP/1.1 ";
    } else {
        os << "HTTP/?.?";
    }
}

void response::deliver_status_code_text(std::ostream & os, const status_code & code)
{
    static const std::map<rest::http::status_code, std::string> status_code_text = {
        {status_code::CONTINUE, "100 Continue"},
        {status_code::SWITCHING_PROTOCOLS, "101 Switching Protocols"},
        {status_code::PROCESSING, "102 Processing"},

        {status_code::OK, "200 Ok"},
        {status_code::CREATED, "201 Continue"},
        {status_code::ACCEPTED, "202 Accepted"},
        {status_code::NON_AUTHORATIVE_INFORMATION, "203 Non Authorative Information"},
        {status_code::NO_CONTENT, "204 No Content"},
        {status_code::RESET_CONTENT, "205 Reset Content"},
        {status_code::PARTIAL_CONTENT, "206 Partial Content"},
        {status_code::MULTI_STATUS, "207 Multi Status"},

        {status_code::MULTIPLE_CHOICES, "300 Multiple Choices"},
        {status_code::MOVED_PERMANENTLY, "301 Moved Permanently"},
        {status_code::FOUND, "302 Found"},
        {status_code::SEE_OTHER, "303 See Other"},
        {status_code::NOT_MODIFIED, "304 Not Modified"},
        {status_code::USE_PROXY, "305 Use Proxy"},
        {status_code::SWITCH_PROXY, "306 Switch Proxy"},
        {status_code::TEMPORARY_REDIRECT, "307 Temporary Redirect"},

        {status_code::BAD_REQUEST, "400 Bad Request"},
        {status_code::UNAUTHORIZED, "401 Unauthorized"},
        {status_code::PAYMENT_REQUIRED, "402 Payment Required"},
        {status_code::FORBIDDEN, "403 Forbidden"},
        {status_code::NOT_FOUND, "404 Not Found"},
        {status_code::METHOD_NOT_ALLOWED, "405 Method Not Allowed"},
        {status_code::METHOD_NOT_ACCEPTABLE, "406 Method Not Acceptable"},
        {status_code::PROXY_AUTHENTIFICATION_REQUIRED, "407 Proxy Authentification Required"},
        {status_code::REQUEST_TIMEOUT, "408 Request Timeout"},
        {status_code::CONFLICT, "409 Conflict"},
        {status_code::GONE, "410 Gone"},
        {status_code::LENGTH_REQUIRED, "411 Length Required"},
        {status_code::PRECONDITION_FAILED, "412 Precondition Failed"},
        {status_code::REQUEST_ENTITY_TOO_LARGE, "413 Request Entity Too Large"},
        {status_code::REQUEST_URI_TOO_LONG, "414 Request Uri Too Long"},
        {status_code::UNSUPPORTED_MEDIA_TYPE, "415 Unsupported Media Type"},
        {status_code::REQUESTED_RANGE_NOT_SATISFIABLE, "416 Requested Range Not Satisfiable"},
        {status_code::EXPECTATION_FAILED, "417 Expectation Failed"},
        {status_code::UNPROCESSABLE_ENTITY, "422 Unprocessable Entity"},
        {status_code::LOCKED, "423 Locked"},
        {status_code::FAILED_DEPENDENCY, "424 Failed Dependency"},
        {status_code::UPGRADE_REQUIRED, "426 Upgrade Required"},

        {status_code::INTERNAL_SERVER_ERROR, "500 Internal Server Error"},
        {status_code::NOT_IMPLEMENTED, "501 Not Implemented"},
        {status_code::BAD_GATEWAY, "502 Bad Gateway"},
        {status_code::SERVICE_UNAVAILABLE, "503 Service Unavailable"},
        {status_code::GATEWAY_TIMEOUT, "504 Gateway Timeout"},
        {status_code::HTTP_VERSION_NOT_SUPPORTED, "505 Http Version Not Supported"}
    };
    auto it = status_code_text.find(code);
    if (it != status_code_text.end()) {
        os << it->second;
    }
}

} // namespace http

} // namespace rest
