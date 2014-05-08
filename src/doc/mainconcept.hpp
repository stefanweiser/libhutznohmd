/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#error "Do not include this pure documentation file."

/*!
@page concept_overview Concept Overview

The library consists mainly of two packages. One with helper classes for a
restful web service and one with a http server. Both are visible for the user and
therefore can be used. This should enable the user to use only specific
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
