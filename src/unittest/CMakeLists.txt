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

SET(FILES   ../mock/demux/mock_handler_manager.hpp
            ../mock/libhutznohmd/mock_communication.hpp
            ../mock/libhutznohmd/mock_demux.hpp
            ../mock/libhutznohmd/mock_request.hpp
            demux/demultiplex_handler.cpp
            demux/demultiplexer.cpp
            demux/demultiplexer_ordered_mime_map.cpp
            request/base64.cpp
            request/lexer.cpp
            request/md5.cpp
            request/memory_allocating_request.cpp
            request/memory_allocating_response.cpp
            request/mime_data.cpp
            request/timestamp.cpp
            request/uri.cpp
            utility/character_validation.cpp
            utility/common.cpp
            utility/parsing.cpp
            utility/select_char_map.cpp
            utility/trie.cpp)

ADD_EXECUTABLE(unittest_hutznohmd ${FILES} ${GMOCK_FILES})
STYLE_FILES(unittest_hutznohmd ${FILES})

SET_SOURCE_FILES_PROPERTIES(${GMOCK_FILES} PROPERTIES COMPILE_FLAGS -w)

TARGET_LINK_LIBRARIES(unittest_hutznohmd hutznohmd)
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/lib")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/mock")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE_DIRECTORIES(SYSTEM "${PROJECT_SOURCE_DIR}/gmock")

INSTALL(TARGETS unittest_hutznohmd DESTINATION bin)
