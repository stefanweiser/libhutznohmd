# This file is part of librestsrv.
# Copyright (C) 2013-2014 Stefan Weiser
#
# The librestsrv project is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3.0 of the License, or (at your
# option) any later version.
#
# The librestsrv project is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.

ENABLE_LANGUAGE(CXX)

INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckCXXCompilerFlag)
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

IF(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
    CHECK_CXX_COMPILER_FLAG("-std=c++11" __CXX11_AVAILABLE__)
ENDIF()

CHECK_STRUCT_HAS_MEMBER("struct linger" l_onoff "sys/socket.h" __HAS_LINGER_ONOFF__)
CHECK_STRUCT_HAS_MEMBER("struct linger" l_linger "sys/socket.h" __HAS_LINGER_LINGER__)
CHECK_STRUCT_HAS_MEMBER("struct pollfd" fd "sys/poll.h" __HAS_POLLFD_FD__)
CHECK_STRUCT_HAS_MEMBER("struct pollfd" events "sys/poll.h" __HAS_POLLFD_EVENTS__)
CHECK_STRUCT_HAS_MEMBER("struct pollfd" revents "sys/poll.h" __HAS_POLLFD_REVENTS__)
CHECK_STRUCT_HAS_MEMBER("struct sockaddr_in" sin_family "netinet/in.h" __HAS_SOCKADDR_IN_FAMILY__)
CHECK_STRUCT_HAS_MEMBER("struct sockaddr_in" sin_port "netinet/in.h" __HAS_SOCKADDR_IN_PORT__)
CHECK_STRUCT_HAS_MEMBER("struct sockaddr_in" sin_addr "netinet/in.h" __HAS_SOCKADDR_IN_ADDR__)
CHECK_STRUCT_HAS_MEMBER("struct in_addr" s_addr "netinet/in.h" __HAS_IN_ADDR_ADDR__)

CHECK_PROTOTYPE_DEFINITION(accept
                           "int accept(int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len)"
                           "0"
                           "sys/socket.h"
                           __HAS_ACCEPT__)
CHECK_PROTOTYPE_DEFINITION(bind
                           "int bind(int __fd, const struct sockaddr *__addr, socklen_t __addrlen)"
                           "0"
                           "sys/socket.h"
                           __HAS_BIND__)
CHECK_PROTOTYPE_DEFINITION(connect
                           "int connect(int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len)"
                           "0"
                           "sys/socket.h"
                           __HAS_CONNECT__)
CHECK_PROTOTYPE_DEFINITION(getsockopt
                           "int getsockopt(int __fd, int __level, int __optname, void *__restrict __optval, socklen_t *__restrict __optlen)"
                           "0"
                           "sys/socket.h"
                           __HAS_GETSOCKOPT__)
CHECK_PROTOTYPE_DEFINITION(listen
                           "int listen(int __fd, int __n)"
                           "0"
                           "sys/socket.h"
                           __HAS_LISTEN__)
CHECK_PROTOTYPE_DEFINITION(recv
                           "ssize_t recv(int __fd, void *__buf, size_t __n, int __flags)"
                           "0"
                           "sys/socket.h"
                           __HAS_RECV__)
CHECK_PROTOTYPE_DEFINITION(send
                           "ssize_t send(int __fd, __const void *__buf, size_t __n, int __flags)"
                           "0"
                           "sys/socket.h"
                           __HAS_SEND__)
CHECK_PROTOTYPE_DEFINITION(setsockopt
                           "int setsockopt(int __fd, int __level, int __optname, __const void *__optval, socklen_t __optlen)"
                           "NULL"
                           "sys/socket.h"
                           __HAS_SETSOCKOPT__)
CHECK_PROTOTYPE_DEFINITION(shutdown
                           "int shutdown(int __fd, int __how)"
                           "0"
                           "sys/socket.h"
                           __HAS_SHUTDOWN__)
CHECK_PROTOTYPE_DEFINITION(socket
                           "int socket(int __domain, int __type, int __protocol)"
                           "0"
                           "sys/socket.h"
                           __HAS_SOCKET__)

CHECK_PROTOTYPE_DEFINITION(close
                           "int close(int __fd)"
                           "0"
                           "unistd.h"
                           __HAS_CLOSE__)
CHECK_PROTOTYPE_DEFINITION(usleep
                           "int usleep(__useconds_t __useconds)"
                           "0"
                           "unistd.h"
                           __HAS_USLEEP__)

CHECK_PROTOTYPE_DEFINITION(htons
                           "uint16_t htons(uint16_t __hostshort)"
                           "0"
                           "arpa/inet.h"
                           __HAS_HTONS__)
CHECK_PROTOTYPE_DEFINITION(inet_aton
                           "int inet_aton(__const char *__cp, struct in_addr *__inp)"
                           "0"
                           "arpa/inet.h"
                           __HAS_INET_ATON__)
CHECK_PROTOTYPE_DEFINITION(ntohs
                           "uint16_t ntohs(uint16_t __netshort)"
                           "0"
                           "arpa/inet.h"
                           __HAS_NTOHS__)
CHECK_PROTOTYPE_DEFINITION(ntohl
                           "uint32_t ntohl(uint32_t __netlong)"
                           "0"
                           "arpa/inet.h"
                           __HAS_NTOHL__)

CHECK_PROTOTYPE_DEFINITION(poll
                           "int poll(struct pollfd *__fds, nfds_t __nfds, int __timeout)"
                           "0"
                           "sys/poll.h"
                           __HAS_POLL__)
