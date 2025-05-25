# This file is part of libhutznohmd.
# Copyright (C) 2013-2025 Stefan Weiser
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

SET(FILES main.cpp)

ADD_EXECUTABLE(example_performance ${FILES})
STYLE_FILES(example_performance ${FILES})

TARGET_LINK_LIBRARIES(example_performance hutznohmd)
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/lib")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

INSTALL(TARGETS example_performance DESTINATION bin)
