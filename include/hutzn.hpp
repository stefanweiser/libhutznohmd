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

#ifndef LIBHUTZNOHMD_HUTZN_HPP
#define LIBHUTZNOHMD_HUTZN_HPP

/*!

@mainpage

The libhutznohmd is a library, that helps the developers of a web service
implementing it according to the principles of the
[Representational State Transfer]
(http://www.ics.uci.edu/~fielding/pubs/dissertation/top.htm "Roy Thomas
Fieldings dissertation") architectural style by providing easy to use, reliable,
robust and well tested (and scalable) functionality. It implements this feature
set on a HTTP/1.1 server. HTTP is defined in RFC 7230-7235 and RFC 7540:

- [RFC 7230 - Message Syntax and Routing](http://tools.ietf.org/html/rfc7230)
- [RFC 7231 - Semantics and Content](http://tools.ietf.org/html/rfc7231)
- [RFC 7232 - Conditional Requests](http://tools.ietf.org/html/rfc7232)
- [RFC 7233 - Range Requests](http://tools.ietf.org/html/rfc7233)
- [RFC 7234 - Caching](http://tools.ietf.org/html/rfc7234)
- [RFC 7235 - Authentication](http://tools.ietf.org/html/rfc7235)
- [RFC 7540 - HTTP/2](http://tools.ietf.org/html/rfc7540)

Currently RFC 7230 and RFC 7231 are supported by this library.

@section sec_general_information General Information

See @subpage page_getting_started to get some examples on how to start building
your own RESTful web service.

See @subpage page_development to get started developing this library.

See @subpage page_about_hutznohmd to learn more about the name of the library
and what it has to do with REST.

@section sec_contact Contact
Stefan Weiser <stefan.weiser@bluewin.ch>

@section sec_license License

@verbatim
GNU LESSER GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.


  This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

  0. Additional Definitions.

  As used herein, "this License" refers to version 3 of the GNU Lesser
General Public License, and the "GNU GPL" refers to version 3 of the GNU
General Public License.

  "The Library" refers to a covered work governed by this License,
other than an Application or a Combined Work as defined below.

  An "Application" is any work that makes use of an interface provided
by the Library, but which is not otherwise based on the Library.
Defining a subclass of a class defined by the Library is deemed a mode
of using an interface provided by the Library.

  A "Combined Work" is a work produced by combining or linking an
Application with the Library.  The particular version of the Library
with which the Combined Work was made is also called the "Linked
Version".

  The "Minimal Corresponding Source" for a Combined Work means the
Corresponding Source for the Combined Work, excluding any source code
for portions of the Combined Work that, considered in isolation, are
based on the Application, and not on the Linked Version.

  The "Corresponding Application Code" for a Combined Work means the
object code and/or source code for the Application, including any data
and utility programs needed for reproducing the Combined Work from the
Application, but excluding the System Libraries of the Combined Work.

  1. Exception to Section 3 of the GNU GPL.

  You may convey a covered work under sections 3 and 4 of this License
without being bound by section 3 of the GNU GPL.

  2. Conveying Modified Versions.

  If you modify a copy of the Library, and, in your modifications, a
facility refers to a function or data to be supplied by an Application
that uses the facility (other than as an argument passed when the
facility is invoked), then you may convey a copy of the modified
version:

   a) under this License, provided that you make a good faith effort to
   ensure that, in the event an Application does not supply the
   function or data, the facility still operates, and performs
   whatever part of its purpose remains meaningful, or

   b) under the GNU GPL, with none of the additional permissions of
   this License applicable to that copy.

  3. Object Code Incorporating Material from Library Header Files.

  The object code form of an Application may incorporate material from
a header file that is part of the Library.  You may convey such object
code under terms of your choice, provided that, if the incorporated
material is not limited to numerical parameters, data structure
layouts and accessors, or small macros, inline functions and templates
(ten or fewer lines in length), you do both of the following:

   a) Give prominent notice with each copy of the object code that the
   Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the object code with a copy of the GNU GPL and this license
   document.

  4. Combined Works.

  You may convey a Combined Work under terms of your choice that,
taken together, effectively do not restrict modification of the
portions of the Library contained in the Combined Work and reverse
engineering for debugging such modifications, if you also do each of
the following:

   a) Give prominent notice with each copy of the Combined Work that
   the Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the Combined Work with a copy of the GNU GPL and this license
   document.

   c) For a Combined Work that displays copyright notices during
   execution, include the copyright notice for the Library among
   these notices, as well as a reference directing the user to the
   copies of the GNU GPL and this license document.

   d) Do one of the following:

       0) Convey the Minimal Corresponding Source under the terms of this
       License, and the Corresponding Application Code in a form
       suitable for, and under terms that permit, the user to
       recombine or relink the Application with a modified version of
       the Linked Version to produce a modified Combined Work, in the
       manner specified by section 6 of the GNU GPL for conveying
       Corresponding Source.

       1) Use a suitable shared library mechanism for linking with the
       Library.  A suitable mechanism is one that (a) uses at run time
       a copy of the Library already present on the user's computer
       system, and (b) will operate properly with a modified version
       of the Library that is interface-compatible with the Linked
       Version.

   e) Provide Installation Information, but only if you would otherwise
   be required to provide such information under section 6 of the
   GNU GPL, and only to the extent that such information is
   necessary to install and execute a modified version of the
   Combined Work produced by recombining or relinking the
   Application with a modified version of the Linked Version. (If
   you use option 4d0, the Installation Information must accompany
   the Minimal Corresponding Source and Corresponding Application
   Code. If you use option 4d1, you must provide the Installation
   Information in the manner specified by section 6 of the GNU GPL
   for conveying Corresponding Source.)

  5. Combined Libraries.

  You may place library facilities that are a work based on the
Library side by side in a single library together with other library
facilities that are not Applications and are not covered by this
License, and convey such a combined library under terms of your
choice, if you do both of the following:

   a) Accompany the combined library with a copy of the same work based
   on the Library, uncombined with any other library facilities,
   conveyed under the terms of this License.

   b) Give prominent notice with the combined library that part of it
   is a work based on the Library, and explaining where to find the
   accompanying uncombined form of the same work.

  6. Revised Versions of the GNU Lesser General Public License.

  The Free Software Foundation may publish revised and/or new versions
of the GNU Lesser General Public License from time to time. Such new
versions will be similar in spirit to the present version, but may
differ in detail to address new problems or concerns.

  Each version is given a distinguishing version number. If the
Library as you received it specifies that a certain numbered version
of the GNU Lesser General Public License "or any later version"
applies to it, you have the option of following the terms and
conditions either of that published version or of any later version
published by the Free Software Foundation. If the Library as you
received it does not specify a version number of the GNU Lesser
General Public License, you may choose any version of the GNU Lesser
General Public License ever published by the Free Software Foundation.

  If the Library as you received it specifies that a proxy can decide
whether future versions of the GNU Lesser General Public License shall
apply, that proxy's public statement of acceptance of any version is
permanent authorization for you to choose that version for the
Library.
@endverbatim


@page page_getting_started Getting Started

@todo [DOC] fill examples


@page page_about_hutznohmd What on earth is a Hutznohmd?

The short story: It was a tradition of the people, who lived in the Ore
Mountains in the past centuries. It was a "communication platform", with its own
set of dos and don'ts, where information about just all ordinary topics were
exchanged. Just what REST is for HTTP, which is the reason to call this library
after the Hutznohmd.

The long story is, that the Ore Mountains (german Erzgebirge, czech Krušné hory)
were populated during the medieval times, because silver and other ores were
discovered there. During the 16th century the Ore Mountains were the heart of
the mining industry in central europe.

In the 17th century economical crisis and especially the Thirty-Years-War
disrupted the mining boom in the Ore Mountains. The population had to find other
occupations. The textile industry and the industry of producing wooden goods and
toys arose out of this. Knipling became increasingly popular. Due to the harsh
climate, to save firewood and because of camaraderie, the women began to work
together in one room. This was the birth of the Hutznstube.

Slowly the Hutzenstube got filled with tradition. The men often joined their
wives after work. They started carving and smoked pipe. The people talked and
sang together. The Hutzenohmd (or Hutzenabend in standard german) was born.
Later artists wrote songs about this tradition. Nowadays the Hutzenabend is
getting marketed more and more as an event including dinner and music.

@section sec_etymology Etymology

The word Abend (Ohmd or Obnd in the dialect of the western Ore Mountains) is
simply the german word for the evening.

The word hutzen (a german weak verb) is a bit complicated, because it is not
used as verb anymore and nowadays all existing sources seem to conflict each
other. In the dialect of the Ore Mountains (as in Vogtland and Franconia) it was
used to say: "to visit people on a stroll".


@page page_development Development

These links may help you to get started developing the library:

- @subpage page_concept
- @subpage page_gurantees
- @subpage page_lifetime
- @subpage page_roadmap

@section sec_principles Development Principles

To reach a homogenous code base, it is indispensable to have some principles
about working with the code. Here are some principles, that are essential for
this library:

- @b KISS: The code has to be as simple as possible. Any non-intuitive solution
for a problem should be refactored or at least documented.
- @b SOLID: This should keep the architecture extensible and robust.
- @b AAA: The tests should be arranged in that pattern, but there are good
reasons that not all tests may fulfill this. A triple-A test is better than one
that is arranged in another way.
- @b YAGNI: Keep this in mind. Don't write code, that you will need in two years
and if some code is not needed anymore, it shall be removed.

In any case it will be difficult where to draw the line. Don't be religious with
these principles, but keep them in mind and improve the code.

@section sec_tools Tools

Generally:
- @b cmake >= 2.8
- @b git >= 1.7.9
- @b python >= 3.2

For building:
- @b clang >= 3.7
- @b clang-format >= 3.7
- @b g++ >= 4.8

Recommended for developing:
- @b qtcreator >= 2.8

For documentation:
- @b doxygen >= 1.8.8
- @b dot >= 2.26
- @b java >= 1.7
- A network connection to download PlantUML in a recent version.

For coverage:
- @b gcov >= 4.8
- @b gcovr >= 3.2

For metrics:
- @b cppcheck >= 1.70
- @b [rats >= 2.4](https://github.com/andrew-d/rough-auditing-tool-for-security)
- @b sonarqube >= 5.3
- @b valgrind >= 3.7
- A network connection to download sonar-runner in a recent version.

For packaging:
- @b tar >= 1.26

@section sec_deployment Deployment

@subsection subsec_quick_start Quick Start

All deployment steps are done by the shell script @c make. A help could be
printed out by:

@code
$ ./make --help
@endcode

Normally making @c build in both targets (@c --debug and @c --release) or making
@c coverage is of interest.

Completed builds, build stability information and nightly builds are available
at the [CI server](https://ci.stefanweiser.de/job/libhutznohmd/). Continuous
inspection of source code quality with a very strict rule set is available at
the [SonarQube server](https://sonar.stefanweiser.de/overview?id=libhutznohmd).

@subsection subsec_deploying Deploying

Deploying a version is done by incrementing the version number in the file @c
version and making a package:

@code
$ ./make --release package
@endcode

The packages will be left in the @c build subdirectory. They ought to be
released. Create a tag on the git repository afterwards.


@page page_concept Concept

Today HTTP is one of the world's most broadly used protocols to connect a server
to its clients. While it is really easy to use and human readable, there is a
need for some rules about the "how to interact". A common "style" is
representational state transfer (REST). The target of this library is to support
its users to fulfill the ideas of REST during development.

Using this library requires some control code and a resource manager to connect
its request handlers to the library. This will make the REST application
available to the clients, who want to request resource representations. The
control code has to listen for and accept the connections, which then are
provided to the library again, that tries to call the correct request handler.

@startuml{use_case.svg} "Use case diagram"
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

Structually the library user needs three things at an abstracted level:

-# An abstraction of the @subpage page_data_source_and_sink
"data source and sink" (e.g. sockets).
-# A @subpage page_demultiplexer "demultiplexing component", that helps to
generate the right response on any request.
-# An access to the @subpage page_requests "request data".

This library solves these needs in separated components. There are interfaces
for socket communication and demultiplexing requests (splitted into two
component groups), but no code to connect those components. The user has to
connect this by own code:

@startuml{components.svg} "Component diagram"
left to right direction
skinparam packageStyle rect

interface listener
interface request_processor
interface demultiplexer

package "libhutznohmd" {
    [listener] as listener
    [request\nprocessor] as request_processor
    [demultiplexer] as demultiplexer
}

package "server" {
    [control code] as control_code
    [resource\nfunction] as resource_function
    [resource\nmanager] as resource_manager
}

listener -[hidden]right- request_processor
control_code ..> listener : listens and accepts
control_code ..> request_processor : provides connections
request_processor ..> resource_function : process\nrequests
request_processor .right.> demultiplexer : determines\nrequest\nhandler
resource_manager ..> demultiplexer : manages resources
@enduml

Therefore the library does not enforce its users to process the requests in a
predefined way. This decision has some advantages and some disadvantages.

Pro:
- more flexible
- modular concept
- easy and functional library implementation

Contra:
- more error potential
- this library is not a whole framework like usually used for web services

Though it is recommended only to connect library components with each other,
this is not enforced. The user is able to write own components to replace those
of the library as long as they fulfill the interface's expectations.


@page page_gurantees Gurantees of the implementation

The implementation gurantees some properties, that get discussed here.

@section sec_exception_safety Exception safety

The library will generally never throw an exception by itself. Raising an
exception has to be qualified as a fatal error for the library code. Therefore
the application should abort and the bug has to get fixed. This enables the user
to choose whether to use exception handling or not. Sadly there is currently no
way to enforce this gurantee without loosing the ability to test with
[googlemock](https://code.google.com/p/googlemock/) (there is currently no way
to define a member function with the noexcept specifier and mock it).

There is also one exception from this rule. The member function
@ref hutzn::request_processor::handle_one_request() may indirectly throw an
exception, because it calls a request handler or an error handler which is not
part of the library's code. The library will not enforce those handler functions
to fulfill this no-throw policy.

@section sec_thread_safety Thread safety

Although the library will not start and stop a thread itself, it is nevertheless
designed to gurantee thread safety everywhere. All functionality could be
accessed simultaneously by multiple threads. This gurantee may introduce
deadlock situations.

As an example there is a deadlock, that happens, if the system is getting
destroyed from within a request handler:

@startuml{most_important_deadlock.svg} "Shutdown deadlock"
left to right direction

(request\nhandler) as rh
(request\nprocessor) as rp
(control\ncode) as cc

cc --> rp: "handle request"
rp --> rh: "calls"
rh --> cc: "stop server"
@enduml

The control code calls the request processor to handle a request. The request
handler is getting called by the request processor, which wants to stop the
server. Stopping the server will wait till all request handlers have finished.


@page page_lifetime Lifetime

@section sec_lifetime_main_objects The main objects of the library

Demultiplexer, request processor and the listener are the main objects used to
build a RESTful web service. These objects are usually constructed once per
program instance, which has to be the first to construct before interacting with
the library and the last to destroy after all interaction objects are cleaned
up. Therefore those objects shall always outlive all other objects.

Note, that a request processor is getting constructed always after the
demultiplexer, because the demultiplexer's query functionality is necessary for
the request processor's construction (the destruction order is indifferent). To
construct such objects simply call the global functions @ref
make_demultiplexer(), @ref make_non_caching_request_processor() and @ref
listen(). They all will return reference-counted objects, that will get
automatically destroyed, when their scope is left.

The following code is an example construction order of those components:
@code{.cpp}
int main()
{
    demux_ptr demultiplexer = make_demultiplexer();
    request_processor_ptr req_processor =
        make_non_caching_request_processor(*demultiplexer);
    listener_ptr listener = listen("0.0.0.0", 8080);

    // do whatever you want...

    // before exiting the function the objects are getting destroyed in the
    // reverse order:
    // destroying listener will stop listening for new requests.
    // destroying request processor disables processing of requests.
    // destroying the demultiplexer will delete the "request handler database".
    return 0;
}
@endcode

@section sec_lifetime_callbacks Callbacks

The user code will interact with the library components mainly by connecting
request handlers and answering requests. When registering a request handler,
an automatically reference counted handler object is returned, which acts as the
registration's scope. When the scope of the handler is left and the object is
getting destroyed, the request handler gets unregistered.

The unregistration procedure is handled senquentially with the calls to the
handler. This is necessary to make real RAII objects out of the handlers, but
introduces another deadlock situation:

@startuml{self_unregistration_deadlock.svg} "Self unregistration deadlock"
left to right direction

(request\nhandler) as rh
(request\nprocessor) as rp
(demultiplexer) as de

rp -> rh: "waits to\nfinish"
rh --> de: "waits for\nunregistration"
de --> rp: "waits for\nusage lock"
@enduml

Therefore it is not allowed for a request handler to unregister itself. To
control the ability of getting called, there are the methods @ref
hutzn::handler::enable() and @ref hutzn::handler::disable().

Note, that the same deadlock problem affects error handlers. Also note, that in
case of the user is exposing its own @c this pointer the handler object must get
destroyed before the object behind the @c this pointer or it will result in
undefined behaviour!

@section sec_lifetime_connection Connection lifetime

When starting a web service by creating a listener, the user of the library has
to call accept. This will wait till a connection is requested and will return
the newly created connection, which is an automatically reference counted
object. The connection will stay open till @ref hutzn::connection::close() is
called or the connection object is released by leaving its scope. This will also
close the connection. As defined for TCP/IP connections, the connection's ports
will then stay open for a "lingering time" to catch stray packets. These states
are called @c TIME_WAIT and @c CLOSE_WAIT.


@page page_roadmap Roadmap

@todo [DOC] define roadmap

HTTP:
- Basic HTTP-Support (protocol, document negotiation)
- Authentification / Authorization
- Conditional-Requests
- Encoding
- Cookies
- Byte Serving
- Caching
- DNT
- HSTS

REST:
- REST helpers
- WADL support

*/

#include <libhutznohmd/demux.hpp>
#include <libhutznohmd/request.hpp>
#include <libhutznohmd/sockets.hpp>
#include <libhutznohmd/types.hpp>

namespace hutzn
{

//! @brief Returns the version number as a string.
//!
//! The library version is configured by cmake.
//! @return Returns a string of the current version.
std::string get_version(void);

} // namespace hutzn

#endif // LIBHUTZNOHMD_HUTZN_HPP
