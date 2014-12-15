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

#ifndef LIBHUTZNOHMD_HUTZN_HPP
#define LIBHUTZNOHMD_HUTZN_HPP

#include <cstdint>
#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <libhutznohmd/demux.hpp>
#include <libhutznohmd/request.hpp>
#include <libhutznohmd/sockets.hpp>

namespace hutzn
{

//! Every HTTP request has a specific method. There are two properties to a
//! subset of methods.
//! 1. Idempotence: You can apply the identical request n times to the same URI
//!    without changing the result.
//! 2. Side-Effect-Safety: The request does not alter the server state. Thus
//!    it is inherently idempotent.
enum class method : uint8_t {
    //! The request is of unknown method. It is recommended to assert this case.
    UNKNOWN = 0,

    //! The HEAD method is identical to GET except that the server must not
    //! return a message-body in the response. The method must not have side
    //! effects.
    HEAD = 1,

    //! The GET method is used to retrieve informations from the entity assigned
    //! to the URI. The method must not have side effects.
    GET = 2,

    //! The PUT method requests to store data to the entity represented by the
    //! URI. This method has to be idempotent.
    PUT = 3,

    //! The DELETE method requests that the origin server deletes the resource
    //! identified by the URI. This method has to be idempotent.
    DELETE = 4,

    //! The POST method is used to request that the origin server accept the
    //! entity enclosed in the request.
    POST = 5,

    //! The TRACE method has to return the message. The client is able to see
    //! whether there are modifications made by intermediate servers.
    TRACE = 6,

    //! The OPTIONS method returns all available methods on the URI.
    OPTIONS = 7,

    //! Converts the request connection to a transparent TCP/IP tunnel, usually
    //! to facilitate SSL-encrypted communication through an unencrypted HTTP
    //! proxy.
    CONNECT = 8
};

namespace http
{

//! These URI schemes are supported by the parser and needed for HTTP.
enum class uri_scheme : uint8_t { UNKNOWN = 0, HTTP = 1, MAILTO = 2 };

//! Contains an URI as used in the HTTP protocol (e.g.
//! http://user\@example.com:8080/?a=b#anchor).
class uri_interface
{
public:
    virtual ~uri_interface();

    //! Returns whether the containing URI is valid or not.
    virtual bool valid() const = 0;

    //! In "http://user\@example.com:8080/?a=b#anchor" this is "http".
    virtual const uri_scheme& scheme() const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "user:pw".
    virtual const char* userinfo() const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "example.com".
    virtual const char* host() const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "8080".
    virtual const uint16_t& port() const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "/".
    virtual const char* path() const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "a=b".
    virtual const char* query() const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "anchor".
    virtual const char* fragment() const = 0;
};

//! A media type is a MIME type as is used to declare how the content of some
//! data is to be read.
class media_type_interface
{
public:
    enum class mime_type : int8_t {
        CUSTOM = -1,
        WILDCARD = 0,
        APPLICATION = 1,
        AUDIO = 2,
        EXAMPLE = 3,
        IMAGE = 4,
        MESSAGE = 5,
        MODEL = 6,
        MULTIPART = 7,
        TEXT = 8,
        VIDEO = 9
    };

    enum class mime_subtype : int16_t { CUSTOM = -1, WILDCARD = 0, PLAIN = 1 };

    virtual ~media_type_interface();

    //! Returns the type, if the media type is known or media_type_type::CUSTOM
    //! if unknown.
    virtual mime_type type() const = 0;

    //! Returns the subtype, if the media type is known or
    //! media_type_subtype::CUSTOM if unknown.
    virtual mime_subtype subtype() const = 0;

    //! Returns the type, if the media type is not known or "" if it is known.
    virtual const char* custom_type() const = 0;

    //! Returns the subtype, if the media type is not known or "" if it is
    //! known.
    virtual const char* custom_subtype() const = 0;

    //! Contains all parameters, that are not parsed specifically (like e.g.
    //! "q").
    virtual const std::map<std::string, std::string>& parameters() const = 0;

    //! The quality is expressed by the parameter "q" of the media type. It
    //! defines, how much desired this media type is.
    virtual uint8_t quality() const = 0;

    //! Returns a value, that defines how specific the media type is. The less
    //! the value, the more unspecific the media type is. These values can be
    //! used to compare media types with each other.
    virtual uint8_t specification_grade() const = 0;
};

//! Defines the HTTP version used by the request.
enum class version {
    //! The request is of unknown version. It is recommended to assert this
    //! case.
    HTTP_UNKNOWN = 0,

    //! The older version of the HTTP protocol.
    HTTP_1_0 = 1,

    //! Main difference to version 1.0 is that servers on one IP address can
    //! host more than one website. The second difference is that persistent
    //! connections are the default case now and non-persistence must be
    //! explicitly wished.
    HTTP_1_1 = 2
};

//! Every request and response has several header fields. Those fields are of a
// specific type.
enum class header_type {
    //! A header, that is not part of the implementation.
    CUSTOM = 0,

    //! A list of accepted mime types. Normally part of the request.
    ACCEPT,

    //! A list of accepted character. Normally part of the request.
    ACCEPT_CHARSET,

    //! A list of accepted encodings. Normally part of the request.
    ACCEPT_ENCODING,

    //! A list of accepted languages. Normally part of the request.
    ACCEPT_LANGUAGE,

    //! If set, the sender supports byte serving for bandwidth optimization.
    // Normally this is only
    //! capable for constant data.
    ACCEPT_RANGES,

    //! List of allowed methods for the requested resource. Part of the
    // response.
    ALLOW,

    //! Contains authorization credentials.
    AUTHORIZATION,

    //! Contains the type of the cache. Controls how the cache works.
    CACHE_CONTROL,

    //! Contains information about what to do after the request is completed.
    CONNECTION,

    //! Contains the encoding of the content.
    CONTENT_ENCODING,

    //! Contains the language of the content.
    CONTENT_LANGUAGE,

    //! Contains the length of the content.
    CONTENT_LENGTH,

    //! Contains the location where the content originally can be found.
    CONTENT_LOCATION,

    //! Contains the md5 sum for the content.
    CONTENT_MD5,

    //! Contains the range of the content.
    CONTENT_RANGE,

    //! Contains the mime type of the content.
    CONTENT_TYPE,

    //! Refers to a cookie.
    COOKIE,

    //! The timestamp, when the message was sent.
    DATE,

    //! Defines an tag for the contained entity. Can be used for conditional
    // requests.
    ETAG,

    //! Includes expectations for the server. If those cannot be met, the server
    // must respond 417.
    EXPECT,

    //! A timestamp, when the response considered stale.
    EXPIRES,

    //! Contains a mailbox entry, which defines who controls source of the
    // request.
    FROM,

    //! Specifies an internet host and maybe a port used for the request.
    HOST,

    //! Responds only if the entity tag matches to the current or any past
    // entity of that resource.
    IF_MATCH,

    //! Responds only if the request variant has been modified since that
    // timestamp.
    IF_MODIFIED_SINCE,

    //! Responds only if the entity tag does not match to the current or any
    // past entity of that
    //! resource.
    IF_NONE_MATCH,

    //! A condition that short circuits a second request, if a conditional
    // request would otherwise
    //! fail.
    IF_RANGE,

    //! The requested operation will be performed, if the resource has been
    // unmodified since that
    //! timestamp.
    IF_UNMODIFIED_SINCE,

    //! Indicated when the resource has been modified last time.
    LAST_MODIFIED,

    //! Should redirect the client to the URL set in that header.
    LOCATION,

    //! Contains application defined parameters.
    PRAGMA,

    //! Contains information about a specific range, that shall be responded.
    RANGE,

    //! Contains an URL, where the client got the requested URL from.
    REFERER,

    //! Indicates, that request could retried and then may succeed.
    RETRY_AFTER,

    //! Contains some information about the used server.
    SERVER,

    //! Sets a cookie at client side. The client could block setting the cookie.
    SET_COOKIE,

    //! Contains some information about the used client.
    USER_AGENT,

    //! Contains a set of header fields, that determine the validity of a cached
    // response.
    VARY,

    //! Contains a challenge with informations for the client on how to
    // authenticate to the server.
    WWW_AUTHENTICATE
};

//! Represents all data, that depend on the request.
class request_interface
{
public:
    virtual ~request_interface();

    //! Requested HTTP method.
    virtual hutzn::method method() const = 0;

    //! Requested URI.
    virtual const uri_interface& request_uri() const = 0;

    //! Requested HTTP version.
    virtual hutzn::http::version version() const = 0;

    //! Returns the custom header of the specified key.
    virtual const std::map<std::string, std::string>& headers() const = 0;

    //! Returns the data buffer.
    virtual const hutzn::buffer& data() const = 0;

    //! Returns the conent type of the data buffer.
    virtual const media_type_interface& data_content_type() const = 0;

    //! Returns the content of the date header or the current timestamp.
    virtual time_t date() const = 0;
};

//! Represents all data, that get transmitted in the response.
class response_interface
{
public:
    virtual ~response_interface();

    //! Sets the status code.
    virtual void
    set_status_code(const hutzn::request::status_code& statusCode) = 0;

    //! Sets the HTTP version.
    virtual void set_version(const hutzn::http::version& version) = 0;

    //! Sets a header by key to a value.
    virtual void set_header(const std::string& key,
                            const std::string& value) = 0;

    //! Sets the content data.
    virtual void set_data(const hutzn::buffer& data) = 0;
};

//! Callback type, used for every http transaction.
using transaction_function = std::function<
    void(const request_interface& request, response_interface& response)>;

//! Represents a HTTP server.
class server_interface
{
public:
    virtual ~server_interface();

    //! Runs the server. Won't return till the server shall stop.
    virtual void run() = 0;

    //! Stops the server from accepting further connections.
    virtual void stop() = 0;
};

//! Shortcut redifinition for a HTTP server.
using server_pointer = std::shared_ptr<server_interface>;

//! Creates a server, that listens at a host address on specific port.
server_pointer create_server(const std::string& host, const uint16_t& port,
                             const transaction_function& access_functor);

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HUTZN_HPP
