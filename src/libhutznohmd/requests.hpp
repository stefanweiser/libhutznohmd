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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_REQUESTS_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_REQUESTS_HPP

namespace hutzn
{

/*!
@page requests Requests

Once you got a connection, your client would propably perform a request. To
parse it and to respond to it correctly a HTTP parser ist needed. HTTP on the
other hand defines access to different documents on the same server. Therefore
a request multiplexer is necessary.

@startuml{requests_classes.png}
namespace hutzn {
  namespace request {
    class representation_identification {
      +url: string
      +verb: method
      +type: mime
    }

    interface request_parser_interface {
      +handle(connection: connection_interface): boolean
    }

    interface handler_interface

    interface multiplexer_interface {
      +register(id: representation_identification, functor): handler_interface
      +set_error_handler(code: status_code, functor): handler_interface
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

Registering a resource representation requires to define a callback function
that must have the signature:

@code{.cpp}
status_code foo(const request_interface&, const response_interface&)
@endcode

Because of @c std::bind you will be able to let the callback function point to
a member function of a class.

@code{.cpp}
std::bind(&Foo::bar, foo_pointer, std::placeholders::_1, std::placeholders::_2);
@endcode

This conceals an important detail from the user. The user exposes the this
pointer of an object. This complicates lifetime. See @ref sec_lifetime_callbacks
"lifetime of callbacks" for further information.
*/

//! This namespace contains all request related code and data. This includes a
//! HTTP parser and a request multiplexer.
namespace request
{

} // namespace request

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_REQUESTS_HPP
