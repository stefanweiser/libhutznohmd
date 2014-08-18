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



# - Find LCOV
# This module finds if LCOV is installed. This code sets the following
# variables:
#
#  LCOV_LCOV_EXECUTABLE = the full path to LCOV
#  LCOV_VERSION_STRING  = Version of the package found (lcov version), eg. 1.10
#  LCOV_VERSION_MAJOR   = The major version of the package found.
#  LCOV_VERSION_MINOR   = The minor version of the package found.
#  LCOV_VERSION         = This is set to: $major.$minor
#
# The minimum required version of LCOV can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(LCOV 1.10)
#
# For these components the following variables are set:
#
#  LCOV_FOUND             - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(LCOV)

FIND_PROGRAM(LCOV_LCOV_EXECUTABLE
  NAMES lcov
  DOC "LCOV (http://ltp.sourceforge.net/coverage/lcov.php)"
)

IF(LCOV_LCOV_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${LCOV_LCOV_EXECUTABLE} --version
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(res)
      IF(${LCOV_FIND_REQUIRED})
        MESSAGE(FATAL_ERROR "Error executing lcov --version")
      ELSE()
        MESSAGE(STATUS "Warning, could not run lcov --version")
      ENDIF()
    ELSE()
      IF(var MATCHES ".*version [0-9]+\\.[0-9]+\\.*")
        STRING(REGEX REPLACE ".*version ([0-9]+\\.[0-9]+).*" "\\1"
               LCOV_VERSION_STRING "${var}")
      ELSE()
        IF(NOT LCOV_FIND_QUIETLY)
          MESSAGE(WARNING "regex not supported: ${var}. Please report")
        ENDIF(NOT LCOV_FIND_QUIETLY)
      ENDIF()
      STRING(REGEX REPLACE "([0-9]+).*" "\\1" LCOV_VERSION_MAJOR "${LCOV_VERSION_STRING}")
      STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" LCOV_VERSION_MINOR "${LCOV_VERSION_STRING}")
      SET(LCOV_VERSION ${LCOV_VERSION_MAJOR}.${LCOV_VERSION_MINOR})
    ENDIF()
ENDIF()

FIND_PROGRAM(LCOV_GENHTML_EXECUTABLE NAMES genhtml)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LCOV
  REQUIRED_VARS LCOV_LCOV_EXECUTABLE
  VERSION_VAR LCOV_VERSION
)

MARK_AS_ADVANCED(
  LCOV_LCOV_EXECUTABLE
  LCOV_GENHTML_EXECUTABLE
)

