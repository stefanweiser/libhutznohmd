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

SET(GENERATED_FILES "${CMAKE_BINARY_DIR}/version.cpp")
SET(FILES   "${PROJECT_SOURCE_DIR}/include/hutzn.hpp"
            "${PROJECT_SOURCE_DIR}/include/libhutznohmd/communication.hpp"
            "${PROJECT_SOURCE_DIR}/include/libhutznohmd/demux.hpp"
            "${PROJECT_SOURCE_DIR}/include/libhutznohmd/request.hpp"
            "${PROJECT_SOURCE_DIR}/include/libhutznohmd/types.hpp"
            communication/internet_socket_connection.cpp
            communication/internet_socket_connection.hpp
            communication/internet_socket_listener.cpp
            communication/internet_socket_listener.hpp
            communication/utility.cpp
            communication/utility.hpp
            demux/demultiplex_handler.cpp
            demux/demultiplex_handler.hpp
            demux/demultiplexer.cpp
            demux/demultiplexer.hpp
            demux/demultiplexer_ordered_mime_map.cpp
            demux/demultiplexer_ordered_mime_map.hpp
            demux/error_handler.cpp
            demux/error_handler.hpp
            demux/error_handler_manager.cpp
            demux/error_handler_manager.hpp
            demux/handler_manager.cpp
            demux/handler_manager.hpp
            demux/request_handler_holder.cpp
            demux/request_handler_holder.hpp
            demux/non_caching_request_processor.cpp
            demux/non_caching_request_processor.hpp
            demux/usage.cpp
            demux/usage.hpp
            libhutznohmd/communication.cpp
            libhutznohmd/demux.cpp
            libhutznohmd/request.cpp
            request/accept_parser.cpp
            request/accept_parser.hpp
            request/base64.cpp
            request/base64.hpp
            request/lexer.cpp
            request/lexer.hpp
            request/md5.cpp
            request/md5.hpp
            request/memory_allocating_request.cpp
            request/memory_allocating_request.hpp
            request/memory_allocating_response.cpp
            request/memory_allocating_response.hpp
            request/mime_data.hpp
            request/mime_handler.cpp
            request/mime_handler.hpp
            request/timestamp.cpp
            request/timestamp.hpp
            request/uri.cpp
            request/uri.hpp
            utility/character_validation.hpp
            utility/common.cpp
            utility/common.hpp
            utility/date_calculation.cpp
            utility/date_calculation.hpp
            utility/parsing.hpp
            utility/select_char_map.hpp
            utility/trie.hpp)

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in"
               "${CMAKE_BINARY_DIR}/version.cpp")
ADD_LIBRARY(hutznohmd SHARED ${FILES} ${GENERATED_FILES})
STYLE_FILES(hutznohmd ${FILES})
SET_TARGET_PROPERTIES(hutznohmd PROPERTIES VERSION "${LIBRARY_VERSION}")

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

INSTALL(TARGETS hutznohmd DESTINATION lib)
