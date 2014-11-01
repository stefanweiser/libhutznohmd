# This file is part of libhutznohmd.
# Copyright (C) 2013-2014 Stefan Weiser
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(libhutznohmd)

IF(NOT UNIX)
    MESSAGE(FATAL_ERROR "This project supports unixoid systems only.")
ENDIF()

GET_FILENAME_COMPONENT(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" REALPATH)
GET_FILENAME_COMPONENT(BUILD_PATH "${CMAKE_BINARY_DIR}" REALPATH)
GET_FILENAME_COMPONENT(GMOCK_PATH "${PROJECT_PATH}/gmock" REALPATH)

SET(CMAKE_MODULE_PATH "${PROJECT_PATH}/cmake" "${PROJECT_PATH}/cmake/packages"
                      ${CMAKE_MODULE_PATH})

INCLUDE(ClangFormat)
INCLUDE(Coverage)
INCLUDE(CPPCheck)
INCLUDE(Documentation)
INCLUDE(Lizard)
INCLUDE(RATS)
INCLUDE(RecursiveSubdirs)
INCLUDE(Toolchain)
INCLUDE(Valgrind)

ENABLE_TESTING()

RECURSIVE_SUBDIRS(src unittest integrationtest examples)
