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



INCLUDE(Toolchain)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-attribute=format")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-attribute=noreturn")

FILE(GLOB_RECURSE SOURCES *.cpp *.c)
FILE(GLOB_RECURSE HEADERS *.hpp *.h)
FILE(GLOB_RECURSE GMOCK ${GMOCK_DIR}/*.cc ${GMOCK_DIR}/*.h)

ADD_EXECUTABLE(integrationtest_restsrv ${SOURCES} ${HEADERS} ${GMOCK})
STYLE_FILES(integrationtest_restsrv ${SOURCES} ${HEADERS})
ADD_TEST(integrationtest "${CMAKE_CURRENT_BINARY_DIR}/integrationtest_restsrv")

LINK_DIRECTORIES("${CMAKE_BINARY_DIR}/lib")
TARGET_LINK_LIBRARIES(integrationtest_restsrv restsrv)

INCLUDE_DIRECTORIES("${PROJECT_DIR}/src")
INCLUDE_DIRECTORIES("${PROJECT_DIR}/integrationtest")
INCLUDE_DIRECTORIES(SYSTEM "${PROJECT_DIR}/gmock")
