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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_DEMUX_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_DEMUX_HPP

#include <cstdint>

#include <libhutznohmd/sockets.hpp>
#include <libhutznohmd/request.hpp>

namespace hutzn
{

/*!

@page page_demultiplexer Demultiplexer

Once a connection is established, the client will send a request. To parse it
and to respond to it correctly a HTTP parser is needed. HTTP on the other hand
defines access to different entities on the same server. Therefore a request
demultiplexer is necessary. It is also required that the user could register and
unregister request and error handlers while processing a request (e.g. when
creating a resource). Therefore processing a request has to be independent from
the demultiplexer.

In other words, requests are heading from the connection through the request
processor to the demultiplexer, which looks for a request handler. This handler
is getting called by the request processor in order to get a response. If no
functor could be found, the request processor will respond an error document.

@startuml{demultiplexer_classes.svg}
namespace hutzn {
  interface block_device_interface

  interface request_interface
  interface response_interface

  class request_handler_id {
    +path: string
    +method: http_verb
    +input_type: mime
    +return_type: mime
  }

  interface handler_interface

  interface request_processor_interface {
    +handle_one_request(device: block_device): bool
    +set_error_handler(reason: http_status_code, fn): handler
  }

  interface demux_query_interface {
    +determine_request_handler(type: mime): fn
  }

  interface demux_interface {
    +connect(id: request_handler_id, fn): handler
    +register_mime_type(type: string, result: mime_type): bool
    +unregister_mime_type(type: mime_type): bool
    +register_mime_subtype(subtype: string, result: mime_subtype): bool
    +unregister_mime_subtype(subtype: mime_subtype): bool
  }

  class handler
  class request_processor
  class demultiplexer

  hutzn.block_device_interface -- request_processor_interface: < uses
  hutzn.request_interface -- request_processor_interface: < uses
  hutzn.response_interface -- request_processor_interface: < uses
  handler_interface -- request_processor_interface: < returns
  request_handler_id -- demux_interface: < uses
  handler_interface -- demux_interface: < returns

  handler_interface <|-- handler: implements
  request_processor_interface <|-- request_processor: implements
  demux_query_interface "1" o-- "1" request_processor
  demux_query_interface <|-- demultiplexer: implements
  demux_interface <|-- demultiplexer: implements
  demux_query_interface <|-- demux_interface: "derives from"
}
@enduml

A resource starts to exist, when the first request handler is getting registered
and ceases when the last request handler is getting destroyed. Registering a
resource handler requires to define a callback function that must have the
signature:

@code{.cpp}
http_status_code foo(const request_interface&,
                     const response_interface&);
@endcode

Because the registration takes a @c std::function the user has the choice to
use a member function (by using @c std::bind) as the callback function.

@code{.cpp}
using namespace std::placeholders;
std::bind(&Foo::bar, foo_pointer, _1, _2);
@endcode

The following example registers a resource handler and sets an error handler
for status code 404:

@code{.cpp}
class C
{
// ...

public:
    http_status_code
    foo(const request_interface&,
        const response_interface&)
    {
        // Do something.
        return http_status_code::OK;
    }

    void error_handler(const request_interface&,
                       const response_interface&)
    {
        // Do something.
    }
};

void register_handlers(C* const c,
                       request_processor_interface& r,
                       demux_interface& m)
{
    request_handler_id i{
        "/",
        http_verb::GET,
        std::make_pair(mime_type::WILDCARD,
                       mime_subtype::WILDCARD),
        std::make_pair(mime_type::TEXT,
                       mime_subtype::PLAIN)
    };

    using namespace std::placeholders;
    m.connect(i, std::bind(&C::foo, c, _1, _2));
    r.set_error_handler(http_status_code::NOT_FOUND,
                        std::bind(&C::error_handler, c, _1, _2));
}
@endcode

Allowing @c std::bind conceals an important detail from the user. The user
could expose the "this" pointer of an object. This complicates lifetime. See
@ref sec_lifetime_callbacks "lifetime of callbacks" for further information.

Every registered handler needs a MIME type. While the library's list is never
complete, it could be extended by the user. You can register own MIME types and
subtypes. There are no lifetime scopes of this registrations, because normally
you want to do the registration and unregistration only once during the
demultiplexer's lifetime.

@code{.cpp}
int main()
{
    demux_pointer d = make_demultiplexer();
    mime_type x_type;
    d->register_mime_type("example", x_type);

    // Do something.

    d->unregister_mime_type(x_type);
    return 0;
}
@endcode

Of course not all combinations of MIME types and subtypes are valid and make
sense. Nevertheless the registration of types and subtypes is indepent, because
it makes the implementation much easier.

*/

//! These informations are used by the demultiplexer to choose the right
//! handler.
struct request_handler_id
{
    //! This string contains only the path of the URL (e.g. "/index.html").
    //! Scheme, authorization, host, port, queries and fragments are not allowed
    //! in this path.
    std::string path;

    //! Only GET, PUT, DELETE and POST are allowed verbs here. All other verbs
    //! are reserved for internal usage.
    http_verb method;

    //! Describes the type that the request handler takes. Will be compared to
    //! the request's content type. INVALID is not allowed here.
    mime content_type;

    //! Describes the type that the request handler returns. Will be matched
    //! to the request's accepted types. INVALID is not allowed here.
    mime accept_type;
};

//! Scopes the request or error handler's lifetime. The handler gets
//! unregistered, when the handler object instance is getting destroyed.
class handler_interface
{
public:
    //! Unregisters the handler.
    virtual ~handler_interface() noexcept(true);
};

//! Handlers are always reference counted.
using handler_pointer = std::shared_ptr<handler_interface>;

//! Is used when the demultiplexer calls a request handler back in order to get
//! a response on a request.
using request_handler_callback = std::function<
    http_status_code(const request_interface&, response_interface&)>;

//! Demultiplexes the requests. It is necessary, that no call to this component
//! blocks its users longer as needed. Any query that currently could not get
//! answered has to result in an error instead of waiting.
class demux_query_interface
{
public:
    //! Do not destroy the demultiplexer while performing any operation on it.
    virtual ~demux_query_interface();

    virtual request_handler_callback determine_request_handler(
        const request_interface& request) = 0;
};

//! Demultiplexers should always be used with reference counted pointers.
using demux_query_pointer = std::shared_ptr<demux_query_interface>;

//! Demultiplexes the requests. It is necessary, that no call to this component
//! blocks its users longer as needed. Any query that currently could not get
//! answered has to result in an error instead of waiting.
class demux_interface : public demux_query_interface
{
public:
    //! Do not destroy the demultiplexer while performing any operation on it.
    virtual ~demux_interface();

    //! Connects a request handler to a resource. Returns a handler object,
    //! which acts as lifetime scope of the request handler. If there is already
    //! a handler with the same request handler id registered, the operation
    //! will fail. It also fails, if the given path is not valid or one of the
    //! used mime types is not registered. When the operation fails it returns
    //! an empty handler.
    virtual handler_pointer connect(const request_handler_id& id,
                                    const request_handler_callback& fn) = 0;

    //! Registers a custom MIME type and returns a new mime_type value if that
    //! type was not already registered. You could use the custom MIME type
    //! afterwards. If the MIME type already exists, it returns
    //! mime_type::INVALID.
    virtual mime_type register_mime_type(const std::string& type) = 0;

    //! Registers a custom MIME subtype and returns a new mime_subtype value if
    //! that type was not already registered. You could use the custom MIME
    //! subtype afterwards. If the MIME subtype already exists, it returns
    //! mime_subtype::INVALID.
    virtual mime_subtype register_mime_subtype(const std::string& subtype) = 0;

    //! Unregisters a MIME type and returns true, if it was found and
    //! successfully unregistered. To successfully unregister a MIME type, it is
    //! necessary, that no registered request handler uses it.
    virtual bool unregister_mime_type(const mime_type& type) = 0;

    //! Unregisters a MIME subtype and returns true, if it was found and
    //! successfully unregistered. To successfully unregister a MIME subtype, it
    //! is necessary, that no registered request handler uses it.
    virtual bool unregister_mime_subtype(const mime_subtype& subtype) = 0;
};

//! Demultiplexers should always be used with reference counted pointers.
using demux_pointer = std::shared_ptr<demux_interface>;

//! Creates a new empty demultiplexer.
demux_pointer make_demultiplexer();

//! Is used by the demultiplexer in case of an error to get a useful response.
using error_handler_callback =
    std::function<void(const request_interface&, response_interface&)>;

//! Waits for, parses and handles the requests. Calls to the request and error
//! handlers. Queries the correct request handler from a given demultiplexer.
class request_processor_interface
{
public:
    //! It is necessary, that no request is getting processed while destroying
    //! it.
    virtual ~request_processor_interface();

    //! Takes a block device to answer one request. Will block until the request
    //! is answered by a request or an error handler. Returns true, if one
    //! request was successfully answered (either as error or not) and false
    //! when the block device got closed during read or send on the connection.
    virtual bool handle_one_request(block_device_interface& device) const = 0;

    //! Connects an error handler to a specific status code. Returns a handler
    //! object, which acts as the error handler's lifetime scope. If there is
    //! already one registered, it returns null.
    virtual handler_pointer set_error_handler(
        const http_status_code& code, const error_handler_callback& fn) = 0;
};

//! The request processor should always be a reference counted pointer
using request_processor_pointer = std::shared_ptr<request_processor_interface>;

//! Creates a new request processor. Needs a query pointer and a connection
//! timeout in seconds.
request_processor_pointer make_request_processor(
    const demux_query_pointer& query_interface,
    const uint64_t& connection_timeout_in_sec = 30);

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_DEMUX_HPP
