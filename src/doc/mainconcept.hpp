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

#error "Do not include this pure documentation file."

/*!

@page concept Concept

Today HTTP is one of the world's most broadly used protocol to connect a server
to its clients. While it is a really easy to use and human readable, there is
a need for some rules about the "how to interact". A common "style" is
representational state transfer (REST). The target of this library is to support
its users to fulfill the ideas of REST.

A user of this library has to provide some control code and a resource manager
to connect its request handlers to the library. This will make the REST
application available to the client, which wants to request resource
representations. The control code has to listen for and accept the connections,
which then are provided to the library again, which tries to call the correct
request handler.

@startuml{use_case.svg}
left to right direction
skinparam packageStyle rect

actor client
actor control_code <<server>>
actor resource_manager <<server>>

rectangle libhutznohmd {
    client -left-> (requests)
    control_code --> (listens)
    control_code --> (accepts\nconnections)
    control_code --> (provides\nconnections)
    resource_manager --> (manages\nresources)

    (requests) .left.> (listens) : <<include>>
    (requests) .right.> (accepts\nconnections) : <<include>>
    (provides\nconnections) .right.> (manages\nresources) : <<include>>
    (provides\nconnections) -[hidden]down- (accepts\nconnections)
}
@enduml

Structually the library user needs two things at an abstracted level:

-# An abstraction of the data source and sink (e.g. sockets).
-# A multiplexing component, that helps to generate the correct response on any
request.

This library is solving this in separated components. There are interfaces for
socket communication and multiplexing requests (splitted into two components),
but no code to connect those components. The user has to connect this by own
code:

@startuml{source_multiplexing_destination.svg}
skinparam component {
    BackgroundColor<<user>> lightblue
    BackgroundColor<<libhutznohmd>> lightyellow
}

[control code] <<user>> as control_code
[listener] <<libhutznohmd>> as listener
[request processor] <<libhutznohmd>> as request_processor
[demultiplexer] <<libhutznohmd>> as demultiplexer
[resource function] <<user>> as resource_function
[resource manager] <<user>> as resource_manager

control_code -down-> listener : listens and accepts
control_code -right-> request_processor : provides connections
request_processor -right-> resource_function : uses to process requests
request_processor -down-> demultiplexer : determines request handler
resource_manager -left-> demultiplexer : manages resources
@enduml

Therefore the library does not enforce its users to process the requests in a
predefined way. This decision has some advantages and some disadvantages.

Pro:
- more flexible
- modular concept
- easy and functional library implementation

Contra:
- more possibilities for errors

Though it is recommended only to connect library components with each other,
this is not enforced. The user is able to write own components to replace those
of the library.

*/
