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



# - Find ClangFormat
# This module finds if ClangFormat is installed. This code sets the following
# variables:
#
#  ClangFormat_CLANGFORMAT_EXECUTABLE = the full path to ClangFormat
#  ClangFormat_VERSION_STRING         = Version of the package found
#                                       (clang-format --version)
#  ClangFormat_VERSION_MAJOR          = The major version of the package found.
#  ClangFormat_VERSION_MINOR          = The minor version of the package found.
#  ClangFormat_VERSION_PATCH          = The patch version of the package found.
#  ClangFormat_VERSION                = This is set to: $major.$minor.$patch
#
# The minimum required version of ClangFormat can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(ClangFormat 3.2)
#
# For these components the following variables are set:
#
#  CLANGFORMAT_FOUND             - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(ClangFormat)

FIND_PROGRAM(ClangFormat_CLANGFORMAT_EXECUTABLE
  NAMES clang-format
)

IF(ClangFormat_CLANGFORMAT_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${ClangFormat_CLANGFORMAT_EXECUTABLE} --version
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(NOT res EQUAL 1)
        IF(${ClangFormat_FIND_REQUIRED})
            MESSAGE(FATAL_ERROR "Error executing clang-format --version")
        ELSE()
            MESSAGE(STATUS "Warning, could not run clang-format --version")
        ENDIF()
    ELSE()
        IF(var MATCHES ".*[0-9]+\\.[0-9]+\\.[0-9]+.*")
            STRING(REGEX REPLACE ".*([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1"
                   ClangFormat_VERSION_STRING "${var}")
        ELSE()
            IF(NOT ClangFormat_FIND_QUIETLY)
                MESSAGE(WARNING "regex not supported: ${var}. Please report")
            ENDIF()
        ENDIF()
        STRING(REGEX REPLACE "([0-9]+).*" "\\1" ClangFormat_VERSION_MAJOR
               "${ClangFormat_VERSION_STRING}")
        STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" ClangFormat_VERSION_MINOR
               "${ClangFormat_VERSION_STRING}")
        STRING(REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1"
               ClangFormat_VERSION_PATCH "${ClangFormat_VERSION_STRING}")
        SET(ClangFormat_VERSION ${ClangFormat_VERSION_MAJOR}.${ClangFormat_VERSION_MINOR})
        SET(ClangFormat_VERSION ${ClangFormat_VERSION}.${ClangFormat_VERSION_PATCH})
    ENDIF()
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ClangFormat
  REQUIRED_VARS ClangFormat_CLANGFORMAT_EXECUTABLE
  VERSION_VAR ClangFormat_VERSION
)

MARK_AS_ADVANCED(
  ClangFormat_CLANGFORMAT_EXECUTABLE
)

