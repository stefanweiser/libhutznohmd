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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_MULTIPLEXER_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_MULTIPLEXER_HPP

#include <libhutznohmd/sockets.hpp>
#include <libhutznohmd/request.hpp>

namespace hutzn
{

/*!

@page multiplexer Multiplexer

Once you got a connection, your client would propably perform a request. To
parse it and to respond to it correctly a HTTP parser ist needed. HTTP on the
other hand defines access to different documents on the same server. Therefore
a request multiplexer is necessary.

@startuml{requests_classes.png}
namespace hutzn {
  namespace request {
    interface media_type_interface

    class request_handler_id {
      +url: string
      +verb: method
      +type: mime_type
      +subtype: mime_subtype
    }

    interface handler_interface

    interface demultiplexer_interface {
      +handle(connection: connection_interface)
      +connect(id: request_handler_id, fn): handler_interface
      +set_error_handler(code: status_code, fn): handler_interface
      +register_mime_type(type: string, result: mime_type): bool
      +unregister_mime_type(type: mime_type): bool
      +register_mime_subtype(subtype: string, result: mime_subtype): bool
      +unregister_mime_subtype(subtype: mime_subtype): bool
    }

    class request_parser
    class handler
    class multiplexer

    handler_interface <|-- handler: <<implements>>
    demultiplexer_interface <|-- multiplexer: <<implements>>
  }
}
@enduml

In other words, requests are heading from the connection through the request
parser to the multiplexer, which looks for a request handler. This handler is
getting called in order to get a response. If no functor could be found, the
request will respond an error document.

A resource starts to exist, when you register the first request handler and
ceases when the last request handler gets destroyed. Registering a resource
handler requires to define a callback function that must have the signature:

@code{.cpp}
status_code foo(const request_interface&, const response_interface&)
@endcode

Because of @c std::bind the user has the choice to use am member function as the
callback function.

@code{.cpp}
std::bind(&Foo::bar, foo_pointer, std::placeholders::_1, std::placeholders::_2);
@endcode

The following example registers a resource representation and sets an error
handler for status code 404:

@code
hutzn::request::status_code C::foo(const hutzn::request::request_interface&,
                                   const hutzn::request::response_interface&)
{
    // Do something.
    return hutzn::request::status_code::OK;
}

void C::error_handler(const hutzn::request::request_interface&,
                      const hutzn::request::response_interface&)
{
    // Do something.
}

void registerHandlers(C* const c, hutzn::multiplexer::demultiplexer_interface&
m)
{
    hutzn::multiplexer::request_handler_id i{
        "/",
        hutzn::request::method::GET,
        hutzn::request::mime_type::WILDCARD,
        hutzn::request::mime_subtype::WILDCARD
    };
    using namespace std::placeholders;
    m.connect(i, std::bind(&C::foo, c, _1, _2));
    m.set_error_handler(hutzn::request::status_code::NOT_FOUND,
                        std::bind(&C::error_handler, c, _1, _2));
}
@endcode

Allowing @c std::bind conceals an important detail from the user. The user
exposes the this pointer of an object. This complicates lifetime. See @ref
sec_lifetime_callbacks "lifetime of callbacks" for further information.

As seen needs every registered handler a mime type. While the list of the
library is never complete,

*/

//! This namespace contains all multiplexer related code and data. This includes
//! the HTTP parser and the request multiplexer.
namespace multiplexer
{

//! These informations are used by the multiplexer to choose the right handler.
struct request_handler_id
{
    //! This string contains only the path of the URL (e.g. "/index.html").
    std::string url;

    //! Only GET, PUT, DELETE and POST are allowed verbs here.
    hutzn::request::method verb;

    //! All known and registered mime types could be used.
    hutzn::request::mime_type type;

    //! All known and registered mime subtypes could be used.
    hutzn::request::mime_subtype subtype;
};

//! Scopes the lifetime of a request or error handler. The handler gets
//! unregistered, when this handler object gets destroyed.
class handler_interface
{
public:
    //! Unregisters the handler.
    virtual ~handler_interface();
};

//! Handlers are always reference counted.
using handler_pointer = std::shared_ptr<handler_interface>;

//! Is used when the multiplexer calls a request handler back in order to get
//! a response on a request.
using request_handler_callback = std::function<
    hutzn::request::status_code(const hutzn::request::request_interface&,
                                hutzn::request::response_interface&)>;

//! Is used by the multiplexer in case of an error to get a useful response.
using error_handler_callback =
    std::function<void(const hutzn::request::request_interface&,
                       hutzn::request::response_interface&)>;

//! Demuxes the HTTP requests to the request handlers.
class demultiplexer_interface
{
public:
    //! While destroying the demuxer, no request must run or this may raise
    //! a segmentation fault.
    virtual ~demultiplexer_interface();

    //! Takes a connection to answer the request on the connection. Will block
    //! until the connection is getting closed.
    virtual void
    handle(const socket::connection_interface& connection) const = 0;

    //! Connects a request handler to a resource. Returns a handler object,
    //! which acts as lifetime scope of the request handler.
    virtual handler_pointer connect(const request_handler_id& id,
                                    const request_handler_callback& fn) = 0;

    //! Connects a error handler to a specific status code. Returns a handler
    //! object, which acts as lifetime scope of the error handler.
    virtual handler_pointer
    set_error_handler(const hutzn::request::status_code& code,
                      const error_handler_callback& fn) = 0;

    //! Registers a custom MIME type and returns true if that type was not
    //! already registered. You could use the custom MIME type afterwards. If
    //! the MIME type already exists, it sets the result to the already
    //! registered one, but returns false.
    virtual bool register_mime_type(const std::string& type,
                                    hutzn::request::mime_type& result) = 0;

    //! Registers a custom MIME subtype and returns true if that type was not
    //! already registered. You could use the custom MIME subtype afterwards. If
    //! the MIME subtype already exists, it sets the result to the already
    //! registered one, but returns false.
    virtual bool
    register_mime_subtype(const std::string& subtype,
                          hutzn::request::mime_subtype& result) = 0;

    //! Unregisters a MIME type and returns true, if it was found and
    //! successfully unregistered. To successfully unregister a MIME type, it is
    //! necessary, that no registered request handler uses it.
    virtual bool
    unregister_mime_type(const hutzn::request::mime_type& type) = 0;

    //! Unregisters a MIME subtype and returns true, if it was found and
    //! successfully unregistered. To successfully unregister a MIME subtype, it
    //! is necessary, that no registered request handler uses it.
    virtual bool
    unregister_mime_subtype(const hutzn::request::mime_subtype& subtype) = 0;
};

} // namespace multiplexer

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_MULTIPLEXER_HPP
