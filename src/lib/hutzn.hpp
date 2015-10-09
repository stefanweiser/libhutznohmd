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
Stefan Weiser <stefan (dot) weiser (at) bluewin (dot) ch>

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
set of dos and don'ts, where informations about simply all ordinary topics were
exchanged. Just what REST is for HTTP, which is the reason to call this helper
library after the Hutznohmd.

The long story is, that the Ore Mountains (german Erzgebirge, czech Krušné hory)
were populated during the medieval times, because silver and other ores were
discovered there. During the 16th century the Ore Mountains were the heart of
the mining industry in central europe.

In the 17th century economical crisis and especially the Thirty-Years-War
disrupted the mining boom in the Ore Mountains. The population had to find other
occupations. The textile industry and the industry of producing wooden goods and
toys arose out of this. Knipling became increasingly popular. Due to the harsh
climate, to save firewood and because of companion, the women began to work
together in one room. This was the birth of the Hutznstube.

Slowly the Hutzenstube got filled with tradition. The men often joined their
wives after work. They started carving, smoked pipe, talked and sang together.
The Hutzenohmd (or Hutzenabend in standard german) was born. Later artists wrote
songs about this tradition. Nowadays the Hutzenabend is getting marketed more
and more as an event including dinner and music.

@section sec_etymology Etymology

The word Abend (Ohmd or Obnd in the dialect of the western Ore Mountains) is
simply the german word for the evening.

The word hutzen (a german weak verb) is a bit complicated, because it is not
used as verb anymore and nowadays all existing sources seem to conflict each
other. In the dialect of the Ore Mountains (as in Vogtland and Franconia) it was
used to say: "to meet people by strolling to them".



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
- @b git >= 1.7.9
- @b cmake >= 2.8

For building:
- @b g++ >= 4.8
- @b clang >= 3.5
- @b cppcheck >= 1.60
- @b clang-format >= 3.5

Recommended for developing:
- @b qtcreator >= 2.8

For documentation:
- @b doxygen >= 1.8.8
- @b dot >= 2.26
- @b java >= 1.6
- A network connection to download PlantUML in a recent version.

For coverage:
- @b gcov >= 4.8
- @b lcov >= 1.9

For valgrind:
- @b valgrind >= 3.7

For metrics:
- @b [lizard >= 1.8.4] (https://pypi.python.org/pypi/lizard)
- @b [rats >= 2.4] (https://code.google.com/p/rough-auditing-tool-for-security/)

For packaging:
- @b tar >= 1.26

@section sec_deployment Deployment

@subsection subsec_quick_start Quick Start

All deployment steps are done by the shell script @c make. A help could be
printed out by:

@code
$ ./make --help
@endcode

Normally making @c all in both targets (@c --debug and @c --release) or making
@c coverage is of interest.

@subsection subsec_deploying Deploying

Deploying a version is done by incrementing the version number in the file @c
version and making a package:

@code
$ ./make --release --minimal build package
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

interface " " as li
interface " " as rpi
interface " " as di

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

li - listener
rpi - request_processor
di - demultiplexer

listener -[hidden]right- request_processor
control_code ..> li : listens and accepts
control_code ..> rpi : provides connections
request_processor ..> resource_function : process\nrequests
request_processor .right.> demultiplexer : determines\nrequest\nhandler
resource_manager ..> di : manages resources
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
@ref request_processor_interface::handle_one_request may indirectly throw an
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
construct such objects simply call the global functions @ref make_demultiplexer,
@ref make_request_processor and @ref listen. They all will return
reference-counted objects, that will get automatically destroyed, when their
scope is left.

The following code is an example construction order of those components:
@code{.cpp}
int main()
{
    demux_pointer demultiplexer = make_demultiplexer();
    request_processor_pointer request_processor =
        make_request_processor(*demultiplexer);
    listener_pointer listener = listen("0.0.0.0", 8080);

    // do whatever needed...

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
handler_interface::enable and @ref handler_interface::disable.

Note, that the same deadlock problem affects error handlers. Also note, that in
case of the user is exposing its own @c this pointer the handler object must get
destroyed before the object behind the @c this pointer or it will result in
undefined behaviour!

@section sec_lifetime_connection Connection lifetime

When starting a web service by creating a listener, the user of the library has
to call accept. This will wait till a connection is requested and will return
the newly created connection, which is an automatically reference counted
object. The connection will stay open till @ref connection_interface::close is
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

//! Every HTTP request has a specific method. There are two properties to a
//! subset of methods.
//! 1. Idempotence: The user can apply the identical request n times to the same
//!    URI without changing the result.
//! 2. Side-Effect-Safety: The request does not alter the server state. Thus
//!    it is inherently idempotent.
enum class method : uint8_t {
    //! The request is of unknown method. It is recommended to assert this case.
    UNKNOWN = 0,

    //! The verb HEAD is identical to GET except that the server must not
    //! return a message-body in the response. This method must not have side
    //! effects.
    HEAD = 1,

    //! The verb GET is used to retrieve informations from the entity assigned
    //! to the URI. The method must not have side effects.
    GET = 2,

    //! The verb PUT requests to store data to the entity represented by the
    //! URI. This method has to be idempotent.
    PUT = 3,

    //! The verb DELETE requests that the origin server deletes the resource
    //! identified by the URI. This method has to be idempotent.
    DELETE = 4,

    //! The verb POST is used to request that the origin server accept the
    //! entity enclosed in the request.
    POST = 5,

    //! The verb TRACE has to return the message. The client is able to see
    //! whether there are modifications made by intermediate servers.
    TRACE = 6,

    //! The verb OPTIONS returns all available methods on the URI.
    OPTIONS = 7,

    //! Converts the request connection to a transparent TCP/IP tunnel, usually
    //! to facilitate SSL-encrypted communication through an unencrypted HTTP
    //! proxy.
    CONNECT = 8
};

namespace http
{

//! These URI schemes are supported by the parser and needed for HTTP.
enum class uri_scheme : uint8_t {
    //! Invalid or unknown value.
    UNKNOWN = 0,

    //! http scheme.
    HTTP = 1,

    //! mailto scheme.
    MAILTO = 2
};

//! Contains a URI as used in the HTTP protocol (e.g.
//! http://user\@example.com:8080/?a=b#anchor).
class uri_interface
{
public:
    //! Virtual destructor.
    virtual ~uri_interface(void);

    //! Returns whether the containing URI is valid or not.
    virtual bool valid(void) const = 0;

    //! In "http://user\@example.com:8080/?a=b#anchor" this is "http".
    virtual const uri_scheme& scheme(void) const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "user:pw".
    virtual const char_t* userinfo(void) const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "example.com".
    virtual const char_t* host(void) const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "8080".
    virtual const uint16_t& port(void) const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "/".
    virtual const char_t* path(void) const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "a=b".
    virtual const char_t* query(void) const = 0;

    //! In "http://user:pw\@example.com:8080/?a=b#anchor" this is "anchor".
    virtual const char_t* fragment(void) const = 0;
};

//! A media type is a MIME type as is used to declare how the content of some
//! data is to be read.
class media_type_interface
{
public:
    //! Legacy mime type.
    enum class mime_type : int8_t {
        //! Custom
        CUSTOM = -1,

        //! Wildcard
        WILDCARD = 0,

        //! Application
        APPLICATION = 1,

        //! Audio
        AUDIO = 2,

        //! Example
        EXAMPLE = 3,

        //! Image
        IMAGE = 4,

        //! Message
        MESSAGE = 5,

        //! Model
        MODEL = 6,

        //! Multipart
        MULTIPART = 7,

        //! Text
        TEXT = 8,

        //! Video
        VIDEO = 9
    };

    //! Legacy mime subtype.
    enum class mime_subtype : int16_t {
        //! Custom
        CUSTOM = -1,

        //! Wildcard
        WILDCARD = 0,

        //! Plain
        PLAIN = 1
    };

    //! Virtual destructor.
    virtual ~media_type_interface(void);

    //! Returns the type, if the media type is known or media_type_type::CUSTOM
    //! if unknown.
    virtual mime_type type(void) const = 0;

    //! Returns the subtype, if the media type is known or
    //! media_type_subtype::CUSTOM if unknown.
    virtual mime_subtype subtype(void) const = 0;

    //! Returns the type, if the media type is not known or "" if it is known.
    virtual const char_t* custom_type(void) const = 0;

    //! Returns the subtype, if the media type is not known or "" if it is
    //! known.
    virtual const char_t* custom_subtype(void) const = 0;

    //! Contains all parameters, that are not parsed specifically (like e.g.
    //! "q").
    virtual const std::map<std::string, std::string>& parameters(
        void) const = 0;

    //! The quality is expressed by the parameter "q" of the media type. It
    //! defines, how much desired this media type is.
    virtual uint8_t quality(void) const = 0;

    //! Returns a value, that defines how specific the media type is. The less
    //! the value, the more unspecific the media type is. These values can be
    //! used to compare media types with each other.
    virtual uint8_t specification_grade(void) const = 0;
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
//! specific type.
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
    //! Normally this is only
    //! capable for constant data.
    ACCEPT_RANGES,

    //! List of allowed methods for the requested resource. Part of the
    //! response.
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

    //! Defines a tag for the contained entity. Can be used for conditional
    //! requests.
    ETAG,

    //! Includes expectations for the server. If those cannot be met, the server
    //! must respond 417.
    EXPECT,

    //! A timestamp, when the response considered stale.
    EXPIRES,

    //! Contains a mailbox entry, which defines who controls source of the
    //! request.
    FROM,

    //! Specifies an internet host and maybe a port used for the request.
    HOST,

    //! Responds only if the entity tag matches to the current or any past
    //! entity of that resource.
    IF_MATCH,

    //! Responds only if the request variant has been modified since that
    //! timestamp.
    IF_MODIFIED_SINCE,

    //! Responds only if the entity tag does not match to the current or any
    //! past entity of that resource.
    IF_NONE_MATCH,

    //! A condition that short circuits a second request, if a conditional
    //! request would otherwise fail.
    IF_RANGE,

    //! The requested operation will be performed, if the resource has been
    //! unmodified since that timestamp.
    IF_UNMODIFIED_SINCE,

    //! Indicated when the resource has been modified last time.
    LAST_MODIFIED,

    //! Should redirect the client to the URL set in that header.
    LOCATION,

    //! Contains application defined parameters.
    PRAGMA,

    //! Contains information about a specific range, that shall be responded.
    RANGE,

    //! Contains a URL, where the client got the requested URL from.
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
    //! response.
    VARY,

    //! Contains a challenge with informations for the client on how to
    //! authenticate to the server.
    WWW_AUTHENTICATE
};

//! Represents all data, that depend on the request.
class request_interface
{
public:
    //! Virtual destructor.
    virtual ~request_interface(void);

    //! Requested HTTP method.
    virtual hutzn::method method(void) const = 0;

    //! Requested URI.
    virtual const uri_interface& request_uri(void) const = 0;

    //! Requested HTTP version.
    virtual http::version version(void) const = 0;

    //! Returns the custom header of the specified key.
    virtual const std::map<std::string, std::string>& headers(void) const = 0;

    //! Returns the data buffer.
    virtual const buffer& data(void) const = 0;

    //! Returns the conent type of the data buffer.
    virtual const media_type_interface& data_content_type(void) const = 0;

    //! Returns the content of the date header or the current timestamp.
    virtual time_t date(void) const = 0;
};

//! Represents all data, that get transmitted in the response.
class response_interface
{
public:
    //! Virtual destructor.
    virtual ~response_interface(void);

    //! Sets the status code.
    virtual void set_status_code(const http_status_code& status_code) = 0;

    //! Sets the HTTP version.
    virtual void set_version(const http::version& version) = 0;

    //! Sets a header by key to a value.
    virtual void set_header(const std::string& key,
                            const std::string& value) = 0;

    //! Sets the content data.
    virtual void set_data(const buffer& data) = 0;
};

//! Callback type, used for every http transaction.
using transaction_function = std::function<
    void(const request_interface& request, response_interface& response)>;

//! Represents a HTTP server.
class server_interface
{
public:
    //! Virtual destructor.
    virtual ~server_interface(void);

    //! Runs the server. Won't return till the server shall stop.
    virtual void run(void) = 0;

    //! Stops the server from accepting further connections.
    virtual void stop(void) = 0;
};

//! Shortcut redifinition for a HTTP server.
using server_pointer = std::shared_ptr<server_interface>;

//! Creates a server, that listens at a host address on specific port.
server_pointer create_server(const std::string& host, const uint16_t& port,
                             const transaction_function& access_functor);

} // namespace http

} // namespace hutzn

#endif // LIBHUTZNOHMD_HUTZN_HPP