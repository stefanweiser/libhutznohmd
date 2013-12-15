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

namespace http
{

//! Every request of HTTP has a specific method.
enum class Method
{
    //! The request is of unknown method.
    UNKNOWN,

    //! The HEAD method is identical to GET except that the server must not
    //! return a message-body in the response. The request must not have side
    //! effects. Though it is idempotent.
    HEAD,

    //! The GET method is used to retrieve informations assigned to the given
    //! URI. The request must not have side effects. Though it is idempotent.
    GET,

    //! The PUT method requests to store data to the entity represented by the
    //! URI. This method has to be idempotent.
    PUT,

    //! The POST method is used to request that the origin server accept the
    //! entity enclosed in the request as a new subordinate of the resource
    //! identified by the Request-URI in the Request-Line.
    POST,

    //! The DELETE method requests that the origin server delete the resource
    //! identified by the Request-URI.
    DELETE,

    //! The TRACE method is used to invoke a remote, application-layer loop-back
    //! of the request message.
    TRACE,

    //! The OPTIONS method represents a request for information about the
    //! communication options available on the request/response chain identified
    //! by the URI.
    OPTIONS,

    //! This specification reserves the method name CONNECT for use with a proxy
    //! that can dynamically switch to being a tunnel.
    CONNECT
};

enum class Version
{
    HTTP_UNKNOWN,
    HTTP_1_0,
    HTTP_1_1
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

//! Function type for a callback, that is called when a client connects.
//! Returning true will accept the connection and false will resuse it.
typedef std::function<bool (const sockaddr *, const socklen_t)>AcceptFn;

//! Function type for a callback, that is called when a client sends a request.
//! Return the status code to answer with. Set the downloadData if you want to
//! send data with the response.
typedef std::function<StatusCode(const std::string& url,
                                 const Method method,
                                 const Version version,
                                 const std::string& uploadData,
                                 std::string& downloadData)>AccessFn;

class IServer
{
public:

    virtual ~IServer() {}

};

//! Creates a http server, that interacts via callback.
IServer createServer(const std::string& address,  //!< Address to bind to.
                     const uint16_t   & port,     //!< Port to bind to.
                     const AcceptFn   & acceptFn, //!< Called if anyone wants
                                                  //!< to connect.
                     const AccessFn   & accessFn  //!< Called if anyone requests
                                                  //!< something.
                     );

} // namespace http

typedef std::function<http::StatusCode(const http::Method&,
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
        const std::vector<http::Method>& method,
        const std::string              & path,
        const RequestFn                & handler) = 0;
};

} // namespace rest

#endif // LIBREST_HPP
