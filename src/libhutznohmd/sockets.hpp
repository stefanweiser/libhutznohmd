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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace hutzn
{

//! @page data_source_and_sink Data source and sink
//!
//! Each restful application has to communicate with its users. To fulfill this
//! necessity this library defines @ref hutzn::socket::connection_interface
//! "connections" and @ref hutzn::socket::listener_interface "listeners", which
//! are the two types of sockets, that are used by network communication
//! protocols. A listener is defined as an endpoint, to which connection
//! endpoints can connect to. So they could be imaginated as half communication
//! channels whose counterparts are missing. Once a connection has been
//! established, there is one connection, that connects two programs via a
//! network. The connection is represented by a connection object.
//!
//! The following example opens the port 80 and listens for exactly one
//! connection, establishs the connection and responses to exactly one HTTP
//! request (that must not contain more than 1024 bytes of payload).
//! Afterwards this happens:
//!
//! -# The connection is getting shut down. The connection resources are getting
//! detached from the process.
//! -# The listener is getting shut down. The listener resources are getting
//! detached from the process.
//! -# The process returns with an exit code of 0.
//! -# Then on most systems the ports, that are used by the connection stay in
//! the state TIME_WAIT to catch stray packets. After a while those ports are
//! getting unused.
//!
//! @code{.cpp}
//! int main()
//! {
//!     auto listener = hutzn::socket::listen("0.0.0.0", 80);
//!     auto connection = listener->accept();
//!     hutzn::buffer request;
//!     if (true == connection->read(request, 1024)) {
//!         hutzn::buffer response;
//!         // parse the http request and build a response.
//!         connection->send(response);
//!     }
//!     return 0;
//! }
//! @endcode
//!
//! As shown above the user has to serve the listen, accept and (indirectly)
//! close operations of the listeners and connections. The user must also manage
//! the connections and listeners (when to store, when to dispose). While by
//! this the user has to write more code, it also leaves the user free to make
//! some important decisions:
//!
//! -# Singlethreaded or multithreaded
//! -# When to accept a connection
//! -# Which connection to accept
//! -# Control over the communicated data

//! Universal data buffer type. Could contain unprintable content or binary
//! data.
using buffer = std::vector<char>;

//! This namespace contains all connection-related code and data. These
//! connections are naturally all about network sockets.
namespace socket
{

//! The term connection is here used as one side of a connected communication
//! channel. This marks the difference to a listener, that defines only a single
//! endpoint and cannot be used for communication directly.
class connection_interface
{
public:
    //! Shuts the connection down if not already done and releases the allocated
    //! resources.
    virtual ~connection_interface();

    //! Shuts down the connection, but remain holding the resources. This will
    //! immediately stop any call on that connection. You could release the
    //! connection object afterwards, because no operation will work on such a
    //! connection.
    virtual void close() = 0;

    //! Invokes a blocking receive operation until something but at most
    //! @c max_size is read from the connection. The buffer will retain its
    //! content and gets extended by the new data. Returning true indicates,
    //! that something has been read. False means, that the connection were
    //! closed in any way. In this case the connection is then useless.
    virtual bool receive(hutzn::buffer& data, const size_t& max_size) = 0;

    //! Invokes a blocking send operation. Returns true if all data were
    //! successfully be sent. In case of a closed connection or a connection
    //! shut down during the send it will return false. In any other cases
    //! the operation will block you.
    virtual bool send(const hutzn::buffer& data) = 0;

    //! @see connection_interface::send(const hutzn::buffer&)
    //! This function behaves equally, but takes a string instead of a binary
    //! buffer.
    virtual bool send(const std::string& data) = 0;

    //! Overwrites the lingering timeout of the connection in seconds. As a part
    //! of most network stacks operating systems keep connections in the state
    //! TIME_WAIT some time after closing the socket (even if the program, that
    //! was associated with that socket, terminated). Usually keeping this
    //! timeouts is a good thing, because TIME_WAIT will eat up stray packets of
    //! old connections. However sometimes it is necessary to overwrite this
    //! timeout (e.g. when integration testing sockets).
    virtual bool set_lingering_timeout(const int& timeout) = 0;

    //! Returns the file descriptor of the underlying socket.
    virtual int socket() const = 0;
};

//! A connection is always handled via reference counted pointers.
using connection_pointer = std::shared_ptr<connection_interface>;

//! A listener is someone, that opens a socket and waits for someone to connect
//! to it. Listeners are not used for communication, but to establish the
//! connection.
class listener_interface
{
public:
    //! Shuts down the listening. Releases all resources afterwars.
    virtual ~listener_interface();

    //! Blocks until someone wants to connect to the listener or the listener
    //! gets closed. In the first case the connection gets established and
    //! returned. In case of closing the listener an empty pointer is getting
    //! returned. The listener gets useless in this case.
    virtual connection_pointer accept() const = 0;

    //! Returns whether the listener is currently listening or not.
    virtual bool listening() const = 0;

    //! Shuts down the listener. This will mean, that any operation gets
    //! immediately stopped. The listener is useless afterwards, because no
    //! operation will succeed.
    virtual void stop() = 0;

    //! @see connection_interface::set_lingering_timeout(const int&)
    virtual bool set_lingering_timeout(const int& timeout) = 0;

    //! Returns the file descriptor of the underlying socket.
    virtual int socket() const = 0;
};

//! A listener is always handled via reference counted pointers.
using listener_pointer = std::shared_ptr<listener_interface>;

//! Creates a listener by host and port, defining the ip address and port number
//! the listener should listen on. It returns a listener object, that do
//! listening on the given host/port combination. You may want to accept the
//! incoming connections afterwards.
listener_pointer listen(const std::string& host, const uint16_t& port);

} // namespace socket

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP
