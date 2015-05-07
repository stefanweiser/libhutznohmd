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

#ifndef LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP
#define LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace hutzn
{

/*!

@page page_data_source_and_sink Data source and sink

Each restful application has to communicate with its users. To fulfill this
necessity this library defines @ref connection_interface "connections"
and @ref listener_interface "listeners", which are the two types of
sockets, that are used by network communication protocols.

A listener is defined as an endpoint, to which connection endpoints can connect
to. Once a connection has been established, it connects two programs. This
connection is represented by a connection object.

@startuml{data_source_sink_classes.svg}
namespace hutzn {
  class buffer <<typedef>>

  interface block_device_interface {
    +receive(data: buffer, max_size: size): boolean
    +send(data: buffer): boolean
    +send(data: string): boolean
  }

  interface connection_interface {
    +close()
    +set_lingering_timeout(timeout: seconds)
    +socket(): file_descriptor
  }

  interface listener_interface {
    +accept(): connection_interface
    +listening(): boolean
    +stop()
    +set_lingering_timeout(timeout: seconds)
    +socket(): file_descriptor
  }

  class connection

  class listener

  block_device_interface <|-- connection_interface
  connection_interface <|-- connection: <<implements>>
  listener_interface <|-- listener: <<implements>>
}
@enduml

The following example opens the port 80 and listens for exactly one
connection, establishs the connection and responses to exactly one HTTP
request (that must not contain more than 1024 bytes of payload).
Afterwards this happens:

-# The connection is getting shut down. The connection resources are getting
detached from the process.
-# The listener is getting shut down. The listener resources are getting
detached from the process.
-# The process returns with an exit code of 0.
-# Then on most systems the ports, that are used by the connection stay in
the state TIME_WAIT to catch stray packets. After a while those ports are
getting unused.

@code{.cpp}
int main()
{
    auto listener = listen("0.0.0.0", 80);
    auto connection = listener->accept();
    buffer request;
    if (true == connection->read(request, 1024)) {
        buffer response;
        // Parse the HTTP request and build a response.
        connection->send(response);
    }
    return 0;
}
@endcode

As shown above the user has to serve the listen, accept and (indirectly)
close operations of the listeners and connections. The user must also manage
the connections and listeners (when and where to store and dispose). This makes
the user write more code on its own, but it also leaves the user free to make
some important decisions:

-# Singlethreaded vs. multithreaded
-# When to accept a connection
-# How many connections to accept
-# Which connection to accept and which to close immediately
-# Control over the communicated data

*/

//! Universal data buffer type. Could contain unprintable content or binary
//! data.
using buffer = std::vector<char>;

//! A block device is an object, which could be used to send and receive data
//! blockwise. These blocks could be of custom size.
class block_device_interface
{
public:
    //! Releases the resources allocated by the block device.
    virtual ~block_device_interface();

    //! Invokes a blocking receive operation until something but at most
    //! @c max_size is read from the connection. The buffer will retain its
    //! content and gets extended by the new data. Returning true indicates,
    //! that something has been read. False means, that the connection were
    //! closed. This makes the connection useless.
    virtual bool receive(buffer& data, const size_t& max_size) = 0;

    //! Invokes a blocking send operation. Returns true when all data were
    //! successfully sent. In case of a closed connection or a connection
    //! shut down during the send it will return false.
    virtual bool send(const buffer& data) = 0;

    //! @see connection_interface::send(const buffer&)
    //! This function behaves equally, but takes a string instead of a binary
    //! buffer.
    virtual bool send(const std::string& data) = 0;
};

//! The term connection is here used as one side of a connected communication
//! channel. This marks the difference to a listener, that defines only a single
//! endpoint and cannot be used for communication directly.
class connection_interface : public block_device_interface
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

    //! Overwrites the lingering timeout of the connection in seconds. As a part
    //! of most network stacks the operating system is keeping connections in
    //! the state TIME_WAIT for some time after closing the socket (even if the
    //! process, that was associated with that socket, terminates). Usually
    //! keeping this timeout at the default value is a good idea, because
    //! TIME_WAIT will eat up stray packets of the old connection. However
    //! sometimes it is necessary to overwrite this timeout (e.g. when
    //! integration testing sockets or this socket implementation).
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
    //! Shuts down the listening. Releases all resources afterwards.
    virtual ~listener_interface();

    //! Blocks until someone wants to connect to the listener or the listener
    //! gets closed. In the first case the connection gets established and
    //! returned. In case of closing the listener an empty pointer is getting
    //! returned and the listener can be released.
    virtual connection_pointer accept() const = 0;

    //! Returns whether the listener is currently listening or not. Naturally
    //! this value is a volatile information.
    virtual bool listening() const = 0;

    //! Shuts down the listener. This means, that any operation gets immediately
    //! stopped and the listener is useless afterwards, because no operation
    //! will succeed if it is shut.
    virtual void stop() = 0;

    //! @see connection_interface::set_lingering_timeout(const int&)
    virtual bool set_lingering_timeout(const int& timeout) = 0;

    //! Returns the file descriptor of the underlying socket.
    virtual int socket() const = 0;
};

//! A listener is always handled via reference counted pointers.
using listener_pointer = std::shared_ptr<listener_interface>;

//! Creates a listener by host and port, defining the ip address and port number
//! the listener should listen on. It returns a listener object, that already
//! listens on the given host/port combination. You may want to accept the
//! incoming connections afterwards.
listener_pointer listen(const std::string& host, const uint16_t& port);

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP
