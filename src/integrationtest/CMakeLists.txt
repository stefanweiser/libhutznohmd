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

SETUP_COMPILER_FLAGS()
DISABLE_GMOCK_DEPENDENT_WARNINGS()

SET(FILES   communication/internet_socket.cpp
            communication/utility.cpp)

ADD_EXECUTABLE(integrationtest_hutznohmd ${FILES} ${GMOCK_FILES})
STYLE_FILES(integrationtest_hutznohmd ${FILES})

SET_SOURCE_FILES_PROPERTIES(${GMOCK_FILES} PROPERTIES COMPILE_FLAGS -w)

TARGET_LINK_LIBRARIES(integrationtest_hutznohmd hutznohmd)
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/lib")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/mock")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE_DIRECTORIES(SYSTEM "${PROJECT_SOURCE_DIR}/gmock")

INSTALL(TARGETS integrationtest_hutznohmd DESTINATION bin)
