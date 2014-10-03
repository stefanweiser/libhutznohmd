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

INCLUDE(CheckCXXCompilerFlag)

IF(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
    CHECK_CXX_COMPILER_FLAG("-std=c++11" __CXX11_AVAILABLE__)
ENDIF()
