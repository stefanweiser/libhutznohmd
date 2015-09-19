# This file is part of libhutznohmd.
# Copyright (C) 2013-2015 Stefan Weiser
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

GET_SOURCE_FILES("files.txt" "${CMAKE_CURRENT_SOURCE_DIR}" FILES)
GET_SOURCE_FILES("${GMOCK_PATH}/files.txt" "${GMOCK_PATH}" GMOCK)
ADD_EXECUTABLE(unittest_hutznohmd ${FILES} ${GMOCK})
STYLE_FILES(unittest_hutznohmd ${FILES})
ADD_TEST(unittest "${CMAKE_CURRENT_BINARY_DIR}/unittest_hutznohmd")

COVERAGE_TARGET(coverage unittest_hutznohmd 'unittest/*')

SET_SOURCE_FILES_PROPERTIES(${GMOCK} PROPERTIES COMPILE_FLAGS -w)

TARGET_LINK_LIBRARIES(unittest_hutznohmd hutznohmd)
INCLUDE_DIRECTORIES("${PROJECT_PATH}/src")
INCLUDE_DIRECTORIES("${PROJECT_PATH}/unittest")
INCLUDE_DIRECTORIES(SYSTEM "${PROJECT_PATH}/gmock")

INSTALL(TARGETS unittest_hutznohmd DESTINATION bin)
