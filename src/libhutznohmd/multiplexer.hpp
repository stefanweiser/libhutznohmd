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

    class representation_identification {
      +url: string
      +verb: method
      +type: mime_type
      +subtype: mime_subtype
    }

    interface request_parser_interface {
      +handle(connection: connection_interface): boolean
    }

    interface handler_interface

    interface multiplexer_interface {
      +connect(id: representation_identification, fn): handler_interface
      +set_error_handler(code: status_code, fn): handler_interface
    }

    class request_parser
    class handler
    class multiplexer

    handler_interface <|-- handler: <<implements>>
    request_parser_interface <|-- request_parser: <<implements>>
    multiplexer_interface <|-- multiplexer: <<implements>>
  }
}
@enduml

In other words, requests are heading from the connection through the request
parser to the multiplexer, which looks for a representation functor. This
functor is getting called in order to get a response. If no functor could be
found, the request will respond an error document.

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

void registerHandlers(C* const c, hutzn::multiplexer::multiplexer_interface& m)
{
    hutzn::multiplexer::representation_identification i{
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

A resource starts to exist, when you register the first representation callback
and ceases when the last representation handler gets destroyed. Registering a
resource representation requires to define a callback function that must have
the signature:

@code{.cpp}
status_code foo(const request_interface&, const response_interface&)
@endcode

Because of @c std::bind the user has the choice to use am member function as the
callback function.

@code{.cpp}
std::bind(&Foo::bar, foo_pointer, std::placeholders::_1, std::placeholders::_2);
@endcode

This conceals an important detail from the user. The user exposes the this
pointer of an object. This complicates lifetime. See @ref sec_lifetime_callbacks
"lifetime of callbacks" for further information.

*/

//! This namespace contains all multiplexer related code and data. This includes
//! the HTTP parser and the request multiplexer.
namespace multiplexer
{

struct representation_identification
{
    std::string url;
    hutzn::request::method verb;
    hutzn::request::mime_type type;
    hutzn::request::mime_subtype subtype;
};

class request_parser_interface
{
public:
    virtual ~request_parser_interface();

    virtual bool handle(const socket::connection_interface& connection) = 0;
};

class handler_interface
{
public:
    virtual ~handler_interface();
};

using handler_pointer = std::shared_ptr<handler_interface>;

using representation_callback = std::function<
    hutzn::request::status_code(const hutzn::request::request_interface&,
                                hutzn::request::response_interface&)>;
using error_callback =
    std::function<void(const hutzn::request::request_interface&,
                       hutzn::request::response_interface&)>;

class multiplexer_interface
{
public:
    virtual ~multiplexer_interface();

    virtual handler_pointer connect(const representation_identification& id,
                                    const representation_callback& fn) = 0;
    virtual handler_pointer
    set_error_handler(const hutzn::request::status_code& code,
                      const error_callback& fn) = 0;
};

} // namespace multiplexer

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_MULTIPLEXER_HPP
