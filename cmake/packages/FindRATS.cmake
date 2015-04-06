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



# - Find RATS
# This module finds if RATS is installed. This code sets the following
# variables:
#
#  RATS_RATS_EXECUTABLE = the full path to RATS
#  RATS_VERSION_STRING  = Version of the package found (rats version)
#  RATS_VERSION_MAJOR   = The major version of the package found.
#  RATS_VERSION_MINOR   = The minor version of the package found.
#  RATS_VERSION         = This is set to: $major.$minor
#
# The minimum required version of RATS can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(RATS 2.4)
#
# For these components the following variables are set:
#
#  RATS_FOUND             - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(RATS)

FIND_PROGRAM(RATS_RATS_EXECUTABLE
  NAMES rats
  DOC "Rough Auditing Tool for Security " +
      "(https://code.google.com/p/rough-auditing-tool-for-security)"
)

IF(RATS_RATS_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${RATS_RATS_EXECUTABLE} -h
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(NOT res EQUAL 1)
        IF(${RATS_FIND_REQUIRED})
            MESSAGE(FATAL_ERROR "Error executing rats -h")
        ELSE()
            MESSAGE(STATUS "Warning, could not run rats -h")
        ENDIF()
    ELSE()
        IF(var MATCHES "RATS v[0-9]+\\.[0-9]+\\.*")
            STRING(REGEX REPLACE "RATS v([0-9]+\\.[0-9]+).*" "\\1"
                   RATS_VERSION_STRING "${var}")
        ELSE()
            IF(NOT RATS_FIND_QUIETLY)
                MESSAGE(WARNING "regex not supported: ${var}. Please report")
            ENDIF()
        ENDIF()
        STRING(REGEX REPLACE "([0-9]+).*" "\\1" RATS_VERSION_MAJOR
               "${RATS_VERSION_STRING}")
        STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" RATS_VERSION_MINOR
               "${RATS_VERSION_STRING}")
        SET(RATS_VERSION ${RATS_VERSION_MAJOR}.${RATS_VERSION_MINOR})
    ENDIF()
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RATS
  REQUIRED_VARS RATS_RATS_EXECUTABLE
  VERSION_VAR RATS_VERSION
)

MARK_AS_ADVANCED(
  RATS_RATS_EXECUTABLE
)

