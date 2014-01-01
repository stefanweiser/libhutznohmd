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
    if (m_version == Version::HTTP_1_0)
    {
        head << "HTTP/1.0 ";
    }
    else if (m_version == Version::HTTP_1_1)
    {
        head << "HTTP/1.1 ";
    }
    else
    {
        throw std::exception();
    }

    head << static_cast<int32_t>(m_statusCode);

    static const std::map<StatusCode, std::string> statusCodeText =
    {
        {StatusCode::Continue, "Continue"},
        {StatusCode::SwitchingProtocols, "Switching Protocols"},
        {StatusCode::Processing, "Processing"},

        {StatusCode::Ok, "Ok"},
        {StatusCode::Created, "Continue"},
        {StatusCode::Accepted, "Accepted"},
        {StatusCode::NonAuthorativeInformation, "Non Authorative Information"},
        {StatusCode::NoContent, "No Content"},
        {StatusCode::ResetContent, "Reset Content"},
        {StatusCode::PartialContent, "Partial Content"},
        {StatusCode::MultiStatus, "Multi Status"},

        {StatusCode::MultipleChoices, "Multiple Choices"},
        {StatusCode::MovedPermanently, "Moved Permanently"},
        {StatusCode::Found, "Found"},
        {StatusCode::SeeOther, "See Other"},
        {StatusCode::NotModified, "Not Modified"},
        {StatusCode::UseProxy, "Use Proxy"},
        {StatusCode::SwitchProxy, "Switch Proxy"},
        {StatusCode::TemporaryRedirect, "Temporary Redirect"},

        {StatusCode::BadRequest, "Bad Request"},
        {StatusCode::Unauthorized, "Unauthorized"},
        {StatusCode::PaymentRequired, "Payment Required"},
        {StatusCode::Forbidden, "Forbidden"},
        {StatusCode::NotFound, "Not Found"},
        {StatusCode::MethodNotAllowed, "Method Not Allowed"},
        {StatusCode::MethodNotAcceptable, "Method Not Acceptable"},
        {StatusCode::ProxyAuthentificationRequired, "Proxy Authentification Required"},
        {StatusCode::RequestTimeout, "Request Timeout"},
        {StatusCode::Conflict, "Conflict"},
        {StatusCode::Gone, "Gone"},
        {StatusCode::LengthRequired, "Length Required"},
        {StatusCode::PreconditionFailed, "Precondition Failed"},
        {StatusCode::RequestEntityTooLarge, "Request Entity Too Large"},
        {StatusCode::RequestUriTooLong, "Request Uri Too Long"},
        {StatusCode::UnsupportedMediaType, "Unsupported Media Type"},
        {StatusCode::RequestedRangeNotSatisfiable, "Requested Range Not Satisfiable"},
        {StatusCode::ExpectationFailed, "Expectation Failed"},
        {StatusCode::UnprocessableEntity, "Unprocessable Entity"},
        {StatusCode::Locked, "Locked"},
        {StatusCode::FailedDependency, "Failed Dependency"},
        {StatusCode::UpgradeRequired, "Upgrade Required"},

        {StatusCode::InternalServerError, "Internal Server Error"},
        {StatusCode::NotImplemented, "Not Implemented"},
        {StatusCode::BadGateway, "Bad Gateway"},
        {StatusCode::ServiceUnavailable, "Service Unavailable"},
        {StatusCode::GatewayTimeout, "Gateway Timeout"},
        {StatusCode::HttpVersionNotSupported, "Http Version Not Supported"}
    };
    auto it = statusCodeText.find(m_statusCode);
    if (it != statusCodeText.end())
    {
        head << " " << it->second;
    }
    head << "\r\n";

    for (const auto & pair : m_headers)
    {
        head << pair.first << ": " << pair.second << "\r\n";
    }
    head << "\r\n";

    std::string str = head.str();
    rest::socket::Buffer buffer(str.begin(), str.end());
    m_connection->send(buffer);
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

void Response::setData(const rest::socket::Buffer & data)
{
    m_data = data;
}

} // namespace http

} // namespace rest
