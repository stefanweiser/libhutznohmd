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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(libhutznohmd)

IF(NOT UNIX)
    MESSAGE(FATAL_ERROR "This project supports unixoid systems only.")
ENDIF()

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
SET(GMOCK_FILES "${PROJECT_SOURCE_DIR}/gmock/gmock/gmock.h"
                "${PROJECT_SOURCE_DIR}/gmock/gtest/gtest.h"
                "${PROJECT_SOURCE_DIR}/gmock/gmock-gtest-all.cc"
                "${PROJECT_SOURCE_DIR}/gmock/gmock_main.cc")

INCLUDE(ClangFormat)
INCLUDE(Toolchain)

ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(src)
