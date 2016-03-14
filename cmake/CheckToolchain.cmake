# This file is part of libhutznohmd.
# Copyright (C) 2013-2016 Stefan Weiser
#
# The libhutznohmd project is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3.0 of the License,
# or (at your option) any later version.
#
# The libhutznohmd project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the libhutznohmd project; if not, see
# <http://www.gnu.org/licenses/>.

ENABLE_LANGUAGE(CXX)

INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckCXXSourceCompiles)
INCLUDE(CheckPrototypeDefinition)
INCLUDE(CheckStructHasMember)



CHECK_INCLUDE_FILE_CXX(arpa/inet.h __HAS_ARPA_INET_H__)
CHECK_INCLUDE_FILE_CXX(fcntl.h __HAS_FCNTL_H__)
CHECK_INCLUDE_FILE_CXX(netdb.h __HAS_NETDB_H__)
CHECK_INCLUDE_FILE_CXX(netinet/in.h __HAS_NETINET_IN_H__)
CHECK_INCLUDE_FILE_CXX(sys/poll.h __HAS_SYS_POLL_H__)
CHECK_INCLUDE_FILE_CXX(sys/socket.h __HAS_SYS_SOCKET_H__)
CHECK_INCLUDE_FILE_CXX(unistd.h __HAS_UNISTD_H__)

IF(NOT __HAS_ARPA_INET_H__ OR
   NOT __HAS_FCNTL_H__ OR
   NOT __HAS_NETDB_H__ OR
   NOT __HAS_NETINET_IN_H__ OR
   NOT __HAS_SYS_POLL_H__ OR
   NOT __HAS_SYS_SOCKET_H__ OR
   NOT __HAS_UNISTD_H__)
    MESSAGE(FATAL_ERROR "One of the required include files were not found.")
ENDIF()



CHECK_STRUCT_HAS_MEMBER("struct linger" l_onoff "sys/socket.h"
                        __HAS_LINGER_ONOFF__)
CHECK_STRUCT_HAS_MEMBER("struct linger" l_linger "sys/socket.h"
                        __HAS_LINGER_LINGER__)
CHECK_STRUCT_HAS_MEMBER("struct pollfd" fd "sys/poll.h"
                        __HAS_POLLFD_FD__)
CHECK_STRUCT_HAS_MEMBER("struct pollfd" events "sys/poll.h"
                        __HAS_POLLFD_EVENTS__)
CHECK_STRUCT_HAS_MEMBER("struct pollfd" revents "sys/poll.h"
                        __HAS_POLLFD_REVENTS__)
CHECK_STRUCT_HAS_MEMBER("struct sockaddr_in" sin_family "netinet/in.h"
                        __HAS_SOCKADDR_IN_FAMILY__)
CHECK_STRUCT_HAS_MEMBER("struct sockaddr_in" sin_port "netinet/in.h"
                        __HAS_SOCKADDR_IN_PORT__)
CHECK_STRUCT_HAS_MEMBER("struct sockaddr_in" sin_addr "netinet/in.h"
                        __HAS_SOCKADDR_IN_ADDR__)
CHECK_STRUCT_HAS_MEMBER("struct in_addr" s_addr "netinet/in.h"
                        __HAS_IN_ADDR_ADDR__)

IF(NOT __HAS_LINGER_ONOFF__ OR
   NOT __HAS_LINGER_LINGER__ OR
   NOT __HAS_POLLFD_FD__ OR
   NOT __HAS_POLLFD_EVENTS__ OR
   NOT __HAS_POLLFD_REVENTS__ OR
   NOT __HAS_SOCKADDR_IN_FAMILY__ OR
   NOT __HAS_SOCKADDR_IN_PORT__ OR
   NOT __HAS_SOCKADDR_IN_ADDR__ OR
   NOT __HAS_IN_ADDR_ADDR__)
    MESSAGE(FATAL_ERROR "One struct is not as expected.")
ENDIF()



CHECK_PROTOTYPE_DEFINITION(accept
                           "int accept(int fd, struct sockaddr *addr, socklen_t *addr_len)"
                           "0"
                           "sys/socket.h"
                           __HAS_ACCEPT__)
CHECK_PROTOTYPE_DEFINITION(bind
                           "int bind(int fd, const struct sockaddr *addr, socklen_t addrlen)"
                           "0"
                           "sys/socket.h"
                           __HAS_BIND__)
CHECK_PROTOTYPE_DEFINITION(connect
                           "int connect(int fd, const struct sockaddr *addr, socklen_t len)"
                           "0"
                           "sys/socket.h"
                           __HAS_CONNECT__)
CHECK_PROTOTYPE_DEFINITION(getsockopt
                           "int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen)"
                           "0"
                           "sys/socket.h"
                           __HAS_GETSOCKOPT__)
CHECK_PROTOTYPE_DEFINITION(listen
                           "int listen(int fd, int n)"
                           "0"
                           "sys/socket.h"
                           __HAS_LISTEN__)
CHECK_PROTOTYPE_DEFINITION(recv
                           "ssize_t recv(int fd, void *buf, size_t n, int flags)"
                           "0"
                           "sys/socket.h"
                           __HAS_RECV__)
CHECK_PROTOTYPE_DEFINITION(send
                           "ssize_t send(int fd, const void *buf, size_t n, int flags)"
                           "0"
                           "sys/socket.h"
                           __HAS_SEND__)
CHECK_PROTOTYPE_DEFINITION(setsockopt
                           "int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)"
                           "0"
                           "sys/socket.h"
                           __HAS_SETSOCKOPT__)
CHECK_PROTOTYPE_DEFINITION(shutdown
                           "int shutdown(int fd, int how)"
                           "0"
                           "sys/socket.h"
                           __HAS_SHUTDOWN__)
CHECK_PROTOTYPE_DEFINITION(socket
                           "int socket(int domain, int type, int protocol)"
                           "0"
                           "sys/socket.h"
                           __HAS_SOCKET__)

CHECK_PROTOTYPE_DEFINITION(close
                           "int close(int fd)"
                           "0"
                           "unistd.h"
                           __HAS_CLOSE__)
CHECK_PROTOTYPE_DEFINITION(usleep
                           "int usleep(useconds_t useconds)"
                           "0"
                           "unistd.h"
                           __HAS_USLEEP__)

CHECK_PROTOTYPE_DEFINITION(htons
                           "uint16_t htons(uint16_t hostshort)"
                           "0"
                           "arpa/inet.h"
                           __HAS_HTONS__)
CHECK_PROTOTYPE_DEFINITION(inet_aton
                           "int inet_aton(const char *cp, struct in_addr *inp)"
                           "0"
                           "arpa/inet.h"
                           __HAS_INET_ATON__)
CHECK_PROTOTYPE_DEFINITION(ntohs
                           "uint16_t ntohs(uint16_t netshort)"
                           "0"
                           "arpa/inet.h"
                           __HAS_NTOHS__)
CHECK_PROTOTYPE_DEFINITION(ntohl
                           "uint32_t ntohl(uint32_t netlong)"
                           "0"
                           "arpa/inet.h"
                           __HAS_NTOHL__)

CHECK_PROTOTYPE_DEFINITION(poll
                           "int poll(struct pollfd *fds, nfds_t nfds, int timeout)"
                           "0"
                           "sys/poll.h"
                           __HAS_POLL__)

IF(NOT __HAS_ACCEPT__ OR
   NOT __HAS_BIND__ OR
   NOT __HAS_CONNECT__ OR
   NOT __HAS_GETSOCKOPT__ OR
   NOT __HAS_LISTEN__ OR
   NOT __HAS_RECV__ OR
   NOT __HAS_SEND__ OR
   NOT __HAS_SETSOCKOPT__ OR
   NOT __HAS_SHUTDOWN__ OR
   NOT __HAS_SOCKET__ OR
   NOT __HAS_CLOSE__ OR
   NOT __HAS_USLEEP__ OR
   NOT __HAS_HTONS__ OR
   NOT __HAS_INET_ATON__ OR
   NOT __HAS_NTOHS__ OR
   NOT __HAS_NTOHL__ OR
   NOT __HAS_POLL__)
    MESSAGE(FATAL_ERROR "One function signature is not as expected.")
ENDIF()
