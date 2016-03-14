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

#include <memory>
#include <string>
#include <vector>

#include <libhutznohmd/types.hpp>

namespace hutzn
{

/*!

@page page_data_source_and_sink Data source and sink

Each restful application has to communicate with its users. To fulfill this
necessity this library defines @ref connection "connections" and @ref listener
"listeners", which are the two types of communication entities, that are
typically used by any interprocess or network communication protocols.

A listener is defined as an endpoint, to which connection endpoints can connect
to. Once a connection has been established, it connects two processes. This
connection is represented by a connection object.

@startuml{data_source_sink_classes.svg} "Socket's class diagram"
namespace hutzn {
  class buffer <<typedef>>

  interface block_device {
    +receive(data: buffer, max_size: size): boolean
    +send(data: buffer): boolean
    +send(data: string): boolean
  }

  interface connection {
    +close()
    +set_lingering_timeout(timeout: seconds)
    +socket(): file_descriptor
  }

  interface listener {
    +accept(): connection
    +listening(): boolean
    +stop()
    +set_lingering_timeout(timeout: seconds)
    +socket(): file_descriptor
  }

  class socket_connection

  class socket_listener

  block_device <|-- socket_connection
  connection <|-- socket_connection: <<implements>>
  listener <|-- socket_listener: <<implements>>
}
@enduml

The following example opens the port 80 and listens for exactly one
connection, establishs the connection and responses to exactly one HTTP
request (that must not contain more than 1024 bytes of payload).
Afterwards this happens:

-# The connection is getting shut down and the connection resources are getting
detached from the process.
-# The listener is getting shut down and the listener resources are getting
detached from the process.
-# The process returns with an exit code of 0.
-# On most systems the ports, that were used by the connection stay in the state
@c TIME_WAIT to catch stray packets before reusing those ports. After a while
those ports are getting unused (note that it could also wait in @c CLOSE_WAIT
when the opposite connection is closed first).

@code{.cpp}
int main()
{
    auto listnr = listen("0.0.0.0", 80);
    auto conn = listnr->accept();
    buffer request;
    if (conn->read(request, 1024)) {
        buffer response;
        // Parse the HTTP request and build a response.
        conn->send(response);
    }
    return 0;
}
@endcode

As shown above the user has to serve the listen, accept and (indirectly)
close operations of the listeners and connections. It must also manage the
connections and listeners (when and where to store and dispose). This makes the
user write more code on its own, but it also leaves the user free to make some
important decisions:

-# Singlethreaded vs. multithreaded
-# When to accept a connection
-# How many connections to accept
-# Which connection to accept and which to close immediately
-# Control over the communicated data

*/

//! Universal data buffer type. Could contain unprintable content or binary
//! data.
using buffer = std::vector<char_t>;

//! @brief An object where data can be received from and send to.
//!
//! The data is always sent blockwise. These blocks could be of custom size.
class block_device
{
public:
    //! @brief Releases the resources allocated by the block device.
    virtual ~block_device(void) noexcept(true);

    //! @brief Invokes a blocking receive operation.
    //!
    //! When something but at most @c max_size is read from the connection, the
    //! call will return. The buffer will retain its content and gets extended
    //! by the new data.
    //! @param[in,out] data     Data buffer to which the data is copied to.
    //! @param[in]     max_size Maximum number of bytes the data buffer gets
    //!                         extended.
    //! @return                 Returning true indicates, that something has
    //!                         been read. False means, that the connection were
    //!                         closed. This makes the connection useless.
    virtual bool receive(buffer& data, const size_t& max_size) = 0;

    //! @brief Invokes a blocking send operation.
    //!
    //! @param[in] data Buffer to send.
    //! @return         Returns true when all data were successfully sent. In
    //!                 case of a closed connection or a connection shut down
    //!                 during the send it will return false.
    virtual bool send(const buffer& data) = 0;

    //! @copydoc connection::send(const buffer&)
    virtual bool send(const std::string& data) = 0;
};

//! @brief Connects to endpoints to receive and send data.
//!
//! The term connection is used here as one side of a connected communication
//! channel. This marks the difference to a listener, that defines only a single
//! endpoint and cannot be used for communication directly.
class connection : public block_device
{
public:
    //! @brief Shuts the connection down.
    //!
    //! Shuts down if not already done and releases the allocated resources.
    virtual ~connection(void) noexcept(true);

    //! @brief Shuts down the connection, but remain holding the resources.
    //!
    //! This will immediately stop any call on that connection. The connection
    //! object could be released afterwards, because no operation will work on
    //! such a connection.
    virtual void close(void) = 0;

    //! @brief Overwrites the lingering timeout of the connection in seconds.
    //!
    //! As a part of most internet network stacks the operating system is
    //! keeping connections in a waiting state for some time after closing the
    //! socket (even if the process, that was associated with that socket,
    //! terminates). Usually keeping this timeout at the default value is a good
    //! idea, because these waiting states will eat up all stray packets of the
    //! old connection. However sometimes it is necessary to overwrite this
    //! timeout (e.g. when writing integration tests).
    //! @param[in] timeout Number of seconds to wait until the port gets ready
    //!                    again.
    //! @return            True, when setting was successful and false on error.
    virtual bool set_lingering_timeout(const int32_t& timeout) = 0;
};

//! A connection is always handled via reference counted pointers.
using connection_ptr = std::shared_ptr<connection>;

//! @brief Occupies an endpoint to wait for clients to connect to it.
//!
//! Listeners are not used for communication, but to establish the connection.
class listener
{
public:
    //! @brief Shuts down the listening.
    //!
    //! Releases all resources of the listener socket afterwards.
    virtual ~listener(void) noexcept(true);

    //! @brief Blocks until someone wants to connect to the listener or the
    //! listener gets closed.
    //!
    //! @return When something not equal to nullptr is returned, someone
    //!         successfully connected to the listener. When the result is equal
    //!         to nullptr, the listener was closed and and therefore can be
    //!         released.
    virtual connection_ptr accept(void) const = 0;

    //! @brief Returns whether the listener is currently listening or not.
    //!
    //! Naturally this value is a volatile information.
    //! @return Returns false, when the listener was closed and true if it still
    //!         listens.
    virtual bool listening(void) const = 0;

    //! @brief Shuts down the listener.
    //!
    //! This means, that any operation gets immediately stopped and the listener
    //! is useless afterwards, because no operation will longer succeed.
    virtual void stop(void) = 0;

    //! @copydoc connection::set_lingering_timeout()
    virtual bool set_lingering_timeout(const int32_t& timeout) = 0;
};

//! A listener is always handled via reference counted pointers.
using listener_ptr = std::shared_ptr<listener>;

//! @brief Creates a listener on an internet socket.
//!
//! It returns a listener object, that already listens on the given internet
//! socket. The incoming connections could get accepted as a next step
//! afterwards.
//! @param[in] host An ip address to listen on. Resolving a dns name is not
//!                 implemented.
//! @param[in] port Port number to use. Note, that often ports below 1024 are
//!                 available to privileged users only.
//! @return         Listener object, that already listens on the given internet
//!                 socket or an empty pointer in any case of error.
listener_ptr listen(const std::string& host, const uint16_t& port);

} // namespace hutzn

#endif // LIBHUTZNOHMD_LIBHUTZNOHMD_SOCKETS_HPP
