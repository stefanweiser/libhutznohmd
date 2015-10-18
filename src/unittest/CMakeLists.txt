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
GET_SOURCE_FILES("${PROJECT_SOURCE_DIR}/gmock/files.txt"
                 "${PROJECT_SOURCE_DIR}/gmock" GMOCK)
ADD_EXECUTABLE(unittest_hutznohmd ${FILES} ${GMOCK})
STYLE_FILES(unittest_hutznohmd ${FILES})

SET_SOURCE_FILES_PROPERTIES(${GMOCK} PROPERTIES COMPILE_FLAGS -w)

TARGET_LINK_LIBRARIES(unittest_hutznohmd hutznohmd)
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/lib")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE_DIRECTORIES(SYSTEM "${PROJECT_SOURCE_DIR}/gmock")

INSTALL(TARGETS unittest_hutznohmd DESTINATION bin)
