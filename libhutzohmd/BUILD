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

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "libhutznohmd",
    srcs = [
        "src/communication/internet_socket_connection.hpp",
        "src/communication/internet_socket_listener.cpp",
        "src/communication/internet_socket_listener.hpp",
        "src/communication/utility.cpp",
        "src/communication/utility.hpp",
        "src/communication/internet_socket_connection.cpp",
        "src/demux/demultiplex_handler.cpp",
        "src/demux/demultiplex_handler.hpp",
        "src/demux/demultiplexer.cpp",
        "src/demux/demultiplexer_ordered_mime_map.cpp",
        "src/demux/demultiplexer_ordered_mime_map.hpp",
        "src/demux/error_handler.cpp",
        "src/demux/error_handler.hpp",
        "src/demux/error_handler_manager.cpp",
        "src/demux/error_handler_manager.hpp",
        "src/demux/handler_manager.cpp",
        "src/demux/handler_manager.hpp",
        "src/demux/non_caching_request_processor.cpp",
        "src/demux/non_caching_request_processor.hpp",
        "src/demux/request_handler_holder.cpp",
        "src/demux/request_handler_holder.hpp",
        "src/demux/usage.cpp",
        "src/demux/usage.hpp",
        "src/demux/demultiplexer.hpp",
        "src/libhutznohmd/communication.cpp",
        "src/libhutznohmd/demux.cpp",
        "src/libhutznohmd/request.cpp",
        "src/request/accept_parser.cpp",
        "src/request/accept_parser.hpp",
        "src/request/base64.hpp",
        "src/request/lexer.cpp",
        "src/request/lexer.hpp",
        "src/request/md5.cpp",
        "src/request/md5.hpp",
        "src/request/memory_allocating_request.hpp",
        "src/request/memory_allocating_response.cpp",
        "src/request/memory_allocating_response.hpp",
        "src/request/mime_data.hpp",
        "src/request/mime_handler.cpp",
        "src/request/mime_handler.hpp",
        "src/request/timestamp.cpp",
        "src/request/timestamp.hpp",
        "src/request/uri.cpp",
        "src/request/uri.hpp",
        "src/request/base64.cpp",
        "src/request/memory_allocating_request.cpp",
        "src/utility/common.cpp",
        "src/utility/date_calculation.cpp",
        "src/utility/date_calculation.hpp",
        "src/utility/parsing.hpp",
        "src/utility/select_char_map.hpp",
        "src/utility/trie.hpp",
        "src/utility/character_validation.hpp",
        "src/utility/common.hpp",
        "src/version.cpp",
    ],
    hdrs = [
        "include/hutzn.hpp",
        "include/libhutznohmd/communication.hpp",
        "include/libhutznohmd/demux.hpp",
        "include/libhutznohmd/request.hpp",
        "include/libhutznohmd/types.hpp",
    ],
    defines = [
        "LIBRARY_VERSION=\"0.0.1\"",
    ],
    copts = ["-Ilibhutzohmd/src"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libhutznohmd_mocks",
    hdrs = [
        "mock/demux/mock_handler_manager.hpp",
        "mock/libhutznohmd/mock_communication.hpp",
        "mock/libhutznohmd/mock_demux.hpp",
        "mock/libhutznohmd/mock_request.hpp",
    ],
    strip_include_prefix = "mock",
    visibility = ["//visibility:public"],
)

cc_test(
    name = "libhutznohmd_unittest",
    srcs = [
        "unittest/demux/demultiplexer.cpp",
        "unittest/demux/demultiplexer_ordered_mime_map.cpp",
        "unittest/demux/demultiplex_handler.cpp",
        "unittest/request/base64.cpp",
        "unittest/request/lexer.cpp",
        "unittest/request/md5.cpp",
        "unittest/request/memory_allocating_request.cpp",
        "unittest/request/memory_allocating_response.cpp",
        "unittest/request/mime_data.cpp",
        "unittest/request/timestamp.cpp",
        "unittest/request/uri.cpp",
        "unittest/utility/parsing.cpp",
        "unittest/utility/select_char_map.cpp",
        "unittest/utility/trie.cpp",
        "unittest/utility/character_validation.cpp",
        "unittest/utility/common.cpp",
    ],
    copts = ["-Ilibhutzohmd/src"],
    deps = [
        ":libhutznohmd",
        ":libhutznohmd_mocks",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "libhutznohmd_integrationtest",
    srcs = [
        "integrationtest/communication/internet_socket.cpp",
        "integrationtest/communication/utility.cpp",
    ],
    copts = ["-Ilibhutzohmd/src"],
    deps = [
        ":libhutznohmd",
        ":libhutznohmd_mocks",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
