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

GET_SOURCE_FILES("files.txt" "${CMAKE_CURRENT_SOURCE_DIR}" FILES)
ADD_EXECUTABLE(example_socket ${FILES})
STYLE_FILES(example_socket ${FILES})

TARGET_LINK_LIBRARIES(example_socket hutznohmd)
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/lib")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

INSTALL(TARGETS example_socket DESTINATION bin)
