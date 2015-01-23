/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2014 Stefan Weiser

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

#include <algorithm>
#include <cctype>
#include <iterator>
#include <set>
#include <sstream>

#include "response.hpp"

namespace hutzn
{

namespace http
{

response::response(const hutzn::socket::connection_pointer& connection)
    : connection_(connection)
    , status_code_(request::status_code::INTERNAL_SERVER_ERROR)
    , version_(version::HTTP_1_1)
    , headers_()
    , data_()
{
}

void response::deliver()
{
    std::stringstream head;
    deliver_version(head, version_);
    deliver_status_code_text(head, status_code_);
    head << "\r\n";

    for (const auto& pair : headers_) {
        head << pair.first << ": " << pair.second << "\r\n";
    }
    head << "\r\n";

    connection_->send(head.str());
    connection_->send(data_);
}

void response::set_status_code(const hutzn::request::status_code& status_code)
{
    status_code_ = status_code;
}

void response::set_version(const hutzn::http::version& version)
{
    version_ = version;
}

void response::set_header(const std::string& key, const std::string& value)
{
    headers_[key] = value;
}

void response::set_data(const hutzn::buffer& data)
{
    data_ = data;
}

void response::deliver_version(std::ostream& os,
                               const hutzn::http::version& version)
{
    if (version == hutzn::http::version::HTTP_1_0) {
        os << "HTTP/1.0 ";
    } else if (version == hutzn::http::version::HTTP_1_1) {
        os << "HTTP/1.1 ";
    } else {
        os << "HTTP/?.?";
    }
}

void response::deliver_status_code_text(std::ostream& os,
                                        const hutzn::request::status_code& code)
{
    static const std::map<hutzn::request::status_code,
                          std::string> status_code_text = {
        {request::status_code::CONTINUE, "100 Continue"},
        {request::status_code::SWITCHING_PROTOCOLS, "101 Switching Protocols"},
        {request::status_code::OK, "200 Ok"},
        {request::status_code::CREATED, "201 Continue"},
        {request::status_code::ACCEPTED, "202 Accepted"},
        {request::status_code::NON_AUTHORATIVE_INFORMATION,
         "203 Non Authorative Information"},
        {request::status_code::NO_CONTENT, "204 No Content"},
        {request::status_code::MULTIPLE_CHOICES, "300 Multiple Choices"},
        {request::status_code::MOVED_PERMANENTLY, "301 Moved Permanently"},
        {request::status_code::FOUND, "302 Found"},
        {request::status_code::SEE_OTHER, "303 See Other"},
        {request::status_code::NOT_MODIFIED, "304 Not Modified"},
        {request::status_code::USE_PROXY, "305 Use Proxy"},
        {request::status_code::TEMPORARY_REDIRECT, "307 Temporary Redirect"},
        {request::status_code::BAD_REQUEST, "400 Bad Request"},
        {request::status_code::UNAUTHORIZED, "401 Unauthorized"},
        {request::status_code::FORBIDDEN, "403 Forbidden"},
        {request::status_code::NOT_FOUND, "404 Not Found"},
        {request::status_code::METHOD_NOT_ALLOWED, "405 Method Not Allowed"},
        {request::status_code::METHOD_NOT_ACCEPTABLE,
         "406 Method Not Acceptable"},
        {request::status_code::PROXY_AUTHENTIFICATION_REQUIRED,
         "407 Proxy Authentification Required"},
        {request::status_code::REQUEST_TIMEOUT, "408 Request Timeout"},
        {request::status_code::CONFLICT, "409 Conflict"},
        {request::status_code::GONE, "410 Gone"},
        {request::status_code::LENGTH_REQUIRED, "411 Length Required"},
        {request::status_code::PRECONDITION_FAILED, "412 Precondition Failed"},
        {request::status_code::REQUEST_ENTITY_TOO_LARGE,
         "413 Request Entity Too Large"},
        {request::status_code::REQUEST_URI_TOO_LONG,
         "414 Request Uri Too Long"},
        {request::status_code::UNSUPPORTED_MEDIA_TYPE,
         "415 Unsupported Media Type"},
        {request::status_code::EXPECTATION_FAILED, "417 Expectation Failed"},
        {request::status_code::UPGRADE_REQUIRED, "426 Upgrade Required"},
        {request::status_code::INTERNAL_SERVER_ERROR,
         "500 Internal Server Error"},
        {request::status_code::NOT_IMPLEMENTED, "501 Not Implemented"},
        {request::status_code::BAD_GATEWAY, "502 Bad Gateway"},
        {request::status_code::SERVICE_UNAVAILABLE, "503 Service Unavailable"},
        {request::status_code::GATEWAY_TIMEOUT, "504 Gateway Timeout"},
        {request::status_code::HTTP_VERSION_NOT_SUPPORTED,
         "505 Http Version Not Supported"}};
    auto it = status_code_text.find(code);
    if (it != status_code_text.end()) {
        os << it->second;
    }
}

} // namespace http

} // namespace hutzn
