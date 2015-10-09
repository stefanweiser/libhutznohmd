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



# - Find gcovr
# This module finds if gcovr is installed. This code sets the following
# variables:
#
#  GCOVR_GCOVR_EXECUTABLE = the full path to gcovr
#  GCOVR_VERSION_STRING   = Version of the package found (gcovr version)
#  GCOVR_VERSION_MAJOR    = The major version of the package found.
#  GCOVR_VERSION_MINOR    = The minor version of the package found.
#  GCOVR_VERSION          = This is set to: $major.$minor
#
# The minimum required version of GCOVR can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(GCovr 3.2)
#
# For these components the following variables are set:
#
#  GCOVR_FOUND - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(GCovr)

FIND_PROGRAM(GCOVR_GCOVR_EXECUTABLE
  NAMES gcovr
  DOC "gcovr (http://gcovr.com/)"
)

IF(GCOVR_GCOVR_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${GCOVR_GCOVR_EXECUTABLE} --version
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(res)
        IF(${GCOVR_FIND_REQUIRED})
            MESSAGE(FATAL_ERROR "Error executing gcovr --version")
        ELSE()
            MESSAGE(STATUS "Warning, could not run gcovr --version")
        ENDIF()
    ELSE()
        IF(var MATCHES ".*gcovr [0-9]+\\.[0-9]+\\.*")
            STRING(REGEX REPLACE ".*gcovr ([0-9]+\\.[0-9]+).*" "\\1"
                   GCOVR_VERSION_STRING "${var}")
        ELSE()
            IF(NOT GCOVR_FIND_QUIETLY)
                MESSAGE(WARNING "regex not supported: ${var}. Please report")
            ENDIF()
        ENDIF()
        STRING(REGEX REPLACE "([0-9]+).*" "\\1" GCOVR_VERSION_MAJOR
               "${GCOVR_VERSION_STRING}")
        STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" GCOVR_VERSION_MINOR
               "${GCOVR_VERSION_STRING}")
        SET(GCOVR_VERSION ${GCOVR_VERSION_MAJOR}.${GCOVR_VERSION_MINOR})
    ENDIF()
ENDIF()

FIND_PROGRAM(GCOVR_GENHTML_EXECUTABLE NAMES genhtml)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(gcovr
  REQUIRED_VARS GCOVR_GCOVR_EXECUTABLE
  VERSION_VAR GCOVR_VERSION
)

MARK_AS_ADVANCED(
  GCOVR_GCOVR_EXECUTABLE
)

