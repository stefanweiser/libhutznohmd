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

#ifndef LIBREST_HPP
#define LIBREST_HPP

#include <functional>
#include <memory>
#include <vector>

namespace rest
{

enum class Method
{
    HEAD,
    GET,
    PUT,
    POST,
    DELETE,
    TRACE,
    OPTIONS,
    CONNECT,
    PATCH
};

enum class StatusCode
{
    Continue                         = 100,
    SwitchingProtocols               = 101,
    Processing                       = 102,
    Ok                               = 200,
    Created                          = 201,
    Accepted                         = 202,
    NonAuthorativeInformation        = 203,
    NoContent                        = 204,
    ResetContent                     = 205,
    PartialContent                   = 206,
    MultiStatus                      = 207,
    MultipleChoices                  = 300,
    MovedPermanently                 = 301,
    Found                            = 302,
    SeeOther                         = 303,
    NotModified                      = 304,
    UseProxy                         = 305,
    SwitchProxy                      = 306,
    TemporaryRedirect                = 307,
    BadRequest                       = 400,
    Unauthorized                     = 401,
    PaymentRequired                  = 402,
    Forbidden                        = 403,
    NotFound                         = 404,
    MethodNotAllowed                 = 405,
    MethodNotAcceptable              = 406,
    ProxyAuthentificationRequired    = 407,
    RequestTimeout                   = 408,
    Conflict                         = 409,
    Gone                             = 410,
    LengthRequired                   = 411,
    PreconditionFailed               = 412,
    RequestEntityTooLarge            = 413,
    RequestUriTooLong                = 414,
    UnsupportedMediaType             = 415,
    RequestedRangeNotSatisfiable     = 416,
    ExpectationFailed                = 417,
    UnprocessableEntity              = 422,
    Locked                           = 423,
    FailedDependency                 = 424,
    UnorderedCollection              = 425,
    UpgradeRequired                  = 426,
    NoResponse                       = 444,
    RetryWith                        = 449,
    BlockedByWindowsParentalControls = 450,
    UnavailableForLegalReasons       = 451,
    InternalServerError              = 500,
    NotImplemented                   = 501,
    BadGateway                       = 502,
    ServiceUnavailable               = 503,
    GatewayTimeout                   = 504,
    HttpVersionNotSupported          = 505,
    HttpVariantAlsoNegotiates        = 506,
    InsufficientStorage              = 507,
    BandwidthLimitExceeded           = 509,
    NotExtended                      = 510
};

typedef std::function<StatusCode(const Method&,
                                 const std::istream&)>
RequestFn;

class ListenerInterface
{
public:

    virtual ~ListenerInterface() {}

};

class HttpServerInterface
{
public:

    virtual ~HttpServerInterface() {}

    virtual std::shared_ptr<ListenerInterface>registerListener(
        const std::vector<Method>& method,
        const std::string        & path,
        const RequestFn          & handler) = 0;
};

} // namespace rest

#endif // LIBREST_HPP
