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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(librestsrv)

GET_FILENAME_COMPONENT(PROJECT_DIR
                       "${CMAKE_CURRENT_SOURCE_DIR}"
                       REALPATH)
GET_FILENAME_COMPONENT(GMOCK_DIR
                       "${PROJECT_DIR}/gmock"
                       REALPATH)
SET(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/packages"
    ${CMAKE_MODULE_PATH})

INCLUDE(ClangFormat)
INCLUDE(Coverage)
INCLUDE(CPPCheck)
INCLUDE(Documentation)
INCLUDE(Lizard)
INCLUDE(RATS)
INCLUDE(Valgrind)

ENABLE_TESTING()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(unittest)
ADD_SUBDIRECTORY(integrationtest)
ADD_SUBDIRECTORY(examples)

SET(CPACK_GENERATOR
    "DEB;RPM;TBZ2;TGZ")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER
    "Stefan Weiser <stefan.weiser@bluewin.ch>")

INCLUDE(CPack)
