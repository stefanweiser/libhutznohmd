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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_DEMUX_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_DEMUX_HPP

#include <libhutznohmd/sockets.hpp>
#include <libhutznohmd/request.hpp>

namespace hutzn
{

/*!

@page demultiplexer Demultiplexer

Once you got a connection, your client would propably perform a request. To
parse it and to respond to it correctly a HTTP parser ist needed. HTTP on the
other hand defines access to different entities on the same server. Therefore
a request demultiplexer is necessary.

@startuml{requests_classes.png}
namespace hutzn {
  namespace request {
    class request_handler_id {
      +path: string
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

    class handler
    class demultiplexer

    handler_interface <|-- handler: <<implements>>
    demultiplexer_interface <|-- demultiplexer: <<implements>>
  }
}
@enduml

In other words, requests are heading from the connection to the demultiplexer,
which looks for a request handler. This handler is getting called in order to
get a response. If no functor could be found, the request will respond an error
document.

A resource starts to exist, when you register the first request handler and
ceases when the last request handler gets destroyed. Registering a resource
handler requires to define a callback function that must have the signature:

@code{.cpp}
status_code foo(const request_interface&, const response_interface&)
@endcode

Because of @c std::function the user has the choice to use a member function (by
using @c std::bind) as the callback function.

@code{.cpp}
std::bind(&Foo::bar, foo_pointer, std::placeholders::_1, std::placeholders::_2);
@endcode

The following example registers a resource representation and sets an error
handler for status code 404:

@code
class C
{
// ...
};

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

void registerHandlers(C* const c, hutzn::demux::demultiplexer_interface& m)
{
    hutzn::demux::request_handler_id i{
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
could expose the "this" pointer of an object. This complicates lifetime. See
@ref sec_lifetime_callbacks "lifetime of callbacks" for further information.

Every registered handler needs a mime type. While the list of the library is
never complete, it could be extended by the user. You can register own MIME
types and subtypes. There are no lifetime scopes of this registrations, because
normally you want to do the registration and unregistration only once during
the lifetime of a demultiplexer.

@code
int main()
{
    using demuxer = std::shared_ptr<hutzn::demux::demultiplexer_interface>;
    demuxer d = demuxer(hutzn::demux::make_demultiplexer());
    hutzn::request::mime_type x_type;
    d->register_mime_type("example", x_type);

    // Do something.

    d->unregister_mime_type(x_type);
    return 0;
}
@endcode

Of course not all combinations of MIME types and subtypes are valid and make
sense. Nevertheless the registration of types and subtypes are indepent,
because it makes the implementation much easier.

*/

//! This namespace contains all demultiplexer related code and data. This
//! includes the HTTP parser and the request demultiplexer.
namespace demux
{

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
    hutzn::request::method verb;

    //! All known and registered mime types could be used.
    hutzn::request::mime_type type;

    //! All known and registered mime subtypes could be used.
    hutzn::request::mime_subtype subtype;
};

//! Scopes the lifetime of a request or error handler. The handler gets
//! unregistered, when the handler object instance of this interface gets
//! destroyed.
class handler_interface
{
public:
    //! Unregisters the handler.
    virtual ~handler_interface();
};

//! Handlers are always reference counted.
using handler_pointer = std::shared_ptr<handler_interface>;

//! Is used when the demultiplexer calls a request handler back in order to get
//! a response on a request.
using request_handler_callback = std::function<
    hutzn::request::status_code(const hutzn::request::request_interface&,
                                hutzn::request::response_interface&)>;

//! Is used by the demultiplexer in case of an error to get a useful response.
using error_handler_callback =
    std::function<void(const hutzn::request::request_interface&,
                       hutzn::request::response_interface&)>;

//! Demuxes the HTTP requests to the request handlers.
class demultiplexer_interface
{
public:
    //! While destroying the demuxer, no request must run or this may raise
    //! a segmentation fault but at least undefined behaviour.
    virtual ~demultiplexer_interface();

    //! Takes a connection to answer the request on the connection. Will block
    //! until the connection is getting closed.
    virtual void handle(const socket::connection_interface& c) const = 0;

    //! Connects a request handler to a resource. Returns a handler object,
    //! which acts as lifetime scope of the request handler.
    virtual handler_pointer connect(const request_handler_id& id,
                                    const request_handler_callback& fn) = 0;

    //! Connects an error handler to a specific status code. Returns a handler
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

//! Creates a new empty demultiplexer. Returns a c pointer! Despite the fact,
//! the this library tries to encourage you to do not use c pointers, it does
//! not know the correct wrapper class. Sometimes a shared and sometimes a
//! unique pointer may be appropriate. The user has to handle this.
demultiplexer_interface* make_demultiplexer();

} // namespace demux

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_DEMUX_HPP
