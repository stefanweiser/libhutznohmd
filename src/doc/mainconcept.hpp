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

@page interaction_concept Interaction concept

To build a restful web service there are two things needed at the most
abstracted level:

-# An abstraction of the source and the destination of data (e.g. sockets).
-# A multiplexing component, that helps to generate the correct response on any
request.

This library is solving this in separated components. There are interfaces for
socket communication and multiplexing requests, but no code to connect those
components. The user has to connect this by own code:

@startuml{source_multiplexing_destination.png}
skinparam component {
    BackgroundColor<<user>> green
    BackgroundColor<<libhutznohmd>> red
}

[control code] <<user>> as control_code
[listener] <<libhutznohmd>> as listener
[request demultiplexer] <<libhutznohmd>> as request_demultiplexer
[resource function] <<user>> as resource_function
[resource manager] <<user>> as resource_manager

control_code -down-> listener : listens and accepts
control_code -right-> request_demultiplexer : provides connection sockets
request_demultiplexer -right-> resource_function : uses to process requests
resource_manager -down-> request_demultiplexer : manages resources
@enduml

Therefore the library does not enforce its users to process the requests in a
predefined way. This decision has some advantages and some disadvantages.

Pro:
- more flexible
- modular concept
- no multithreading in the library

Contra:
- more possibilities for errors

Though it is recommended only to connect library components with each other,
this is not enforced.

@page concept_overview Concept Overview

The library consists mainly of two packages. One with helper classes for a
restful web service and one with a http server. Both are visible for the user
and therefore can be used. This should enable the user to use only specific
components.

@startuml{main_concept.png}
package "rest" {
    [container]
    [resource]
}

package "http-server" {
    [request]
    [http-parser]
    [response]
}
@enduml

*/
