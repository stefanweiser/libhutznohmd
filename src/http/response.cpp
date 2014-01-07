/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

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
#include <iostream>
#include <iomanip>
#include <iterator>
#include <set>
#include <sstream>

#include "response.hpp"

namespace rest
{

namespace http
{

Response::Response(const rest::socket::ConnectionPtr & connection)
    : m_connection(connection)
    , m_statusCode(StatusCode::InternalServerError)
    , m_version(Version::HTTP_1_1)
    , m_headers()
    , m_data()
{}

void Response::deliver()
{
    std::stringstream head;
    deliverVersion(head, m_version);
    deliverStatusCodeText(head, m_statusCode);
    head << "\r\n";

    for (const auto & pair : m_headers)
    {
        head << pair.first << ": " << pair.second << "\r\n";
    }
    head << "\r\n";

    m_connection->send(head.str());
    m_connection->send(m_data);
}

void Response::setStatusCode(const StatusCode & statusCode)
{
    m_statusCode = statusCode;
}

void Response::setVersion(const Version & version)
{
    m_version = version;
}

void Response::setHeader(const std::string & key, const std::string & value)
{
    m_headers[key] = value;
}

void Response::setData(const rest::Buffer & data)
{
    m_data = data;
}

void Response::deliverVersion(std::ostream & os, const Version & version)
{
    if (version == Version::HTTP_1_0)
    {
        os << "HTTP/1.0 ";
    }
    else if (version == Version::HTTP_1_1)
    {
        os << "HTTP/1.1 ";
    }
    else
    {
        throw std::exception();
    }
}

void Response::deliverStatusCodeText(std::ostream & os, const StatusCode & statusCode)
{
    static const std::map<StatusCode, std::string> statusCodeText =
    {
        {StatusCode::Continue, "100 Continue"},
        {StatusCode::SwitchingProtocols, "101 Switching Protocols"},
        {StatusCode::Processing, "102 Processing"},

        {StatusCode::Ok, "200 Ok"},
        {StatusCode::Created, "201 Continue"},
        {StatusCode::Accepted, "202 Accepted"},
        {StatusCode::NonAuthorativeInformation, "203 Non Authorative Information"},
        {StatusCode::NoContent, "204 No Content"},
        {StatusCode::ResetContent, "205 Reset Content"},
        {StatusCode::PartialContent, "206 Partial Content"},
        {StatusCode::MultiStatus, "207 Multi Status"},

        {StatusCode::MultipleChoices, "300 Multiple Choices"},
        {StatusCode::MovedPermanently, "301 Moved Permanently"},
        {StatusCode::Found, "302 Found"},
        {StatusCode::SeeOther, "303 See Other"},
        {StatusCode::NotModified, "304 Not Modified"},
        {StatusCode::UseProxy, "305 Use Proxy"},
        {StatusCode::SwitchProxy, "306 Switch Proxy"},
        {StatusCode::TemporaryRedirect, "307 Temporary Redirect"},

        {StatusCode::BadRequest, "400 Bad Request"},
        {StatusCode::Unauthorized, "401 Unauthorized"},
        {StatusCode::PaymentRequired, "402 Payment Required"},
        {StatusCode::Forbidden, "403 Forbidden"},
        {StatusCode::NotFound, "404 Not Found"},
        {StatusCode::MethodNotAllowed, "405 Method Not Allowed"},
        {StatusCode::MethodNotAcceptable, "406 Method Not Acceptable"},
        {StatusCode::ProxyAuthentificationRequired, "407 Proxy Authentification Required"},
        {StatusCode::RequestTimeout, "408 Request Timeout"},
        {StatusCode::Conflict, "409 Conflict"},
        {StatusCode::Gone, "410 Gone"},
        {StatusCode::LengthRequired, "411 Length Required"},
        {StatusCode::PreconditionFailed, "412 Precondition Failed"},
        {StatusCode::RequestEntityTooLarge, "413 Request Entity Too Large"},
        {StatusCode::RequestUriTooLong, "414 Request Uri Too Long"},
        {StatusCode::UnsupportedMediaType, "415 Unsupported Media Type"},
        {StatusCode::RequestedRangeNotSatisfiable, "416 Requested Range Not Satisfiable"},
        {StatusCode::ExpectationFailed, "417 Expectation Failed"},
        {StatusCode::UnprocessableEntity, "422 Unprocessable Entity"},
        {StatusCode::Locked, "423 Locked"},
        {StatusCode::FailedDependency, "424 Failed Dependency"},
        {StatusCode::UpgradeRequired, "426 Upgrade Required"},

        {StatusCode::InternalServerError, "500 Internal Server Error"},
        {StatusCode::NotImplemented, "501 Not Implemented"},
        {StatusCode::BadGateway, "502 Bad Gateway"},
        {StatusCode::ServiceUnavailable, "503 Service Unavailable"},
        {StatusCode::GatewayTimeout, "504 Gateway Timeout"},
        {StatusCode::HttpVersionNotSupported, "505 Http Version Not Supported"}
    };
    auto it = statusCodeText.find(statusCode);
    if (it != statusCodeText.end())
    {
        os << it->second;
    }
}

} // namespace http

} // namespace rest
