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



# - Find CPPCheck
# This module finds if CPPCheck is installed. This code sets the following
# variables:
#
#  CPPCheck_CPPCHECK_EXECUTABLE = the full path to CPPCheck
#  CPPCheck_VERSION_STRING      = Version of the package found (cppcheck version), eg. 1.60
#  CPPCheck_VERSION_MAJOR       = The major version of the package found.
#  CPPCheck_VERSION_MINOR       = The minor version of the package found.
#  CPPCheck_VERSION             = This is set to: $major.$minor
#
# The minimum required version of CPPCheck can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(CPPCheck 1.60)
#
# For these components the following variables are set:
#
#  CPPCheck_FOUND             - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(CPPCheck)

FIND_PROGRAM(CPPCheck_CPPCHECK_EXECUTABLE
  NAMES cppcheck
  DOC "Cppcheck (http://cppcheck.sourceforge.net)"
)

IF(CPPCheck_CPPCHECK_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${CPPCheck_CPPCHECK_EXECUTABLE} --version
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(res)
      IF(${CPPCheck_FIND_REQUIRED})
        MESSAGE(FATAL_ERROR "Error executing astyle --version")
      ELSE()
        MESSAGE(STATUS "Warning, could not run astyle --version")
      ENDIF()
    ELSE()
      IF(var MATCHES ".* [0-9]+\\.[0-9]+\\.*")
        STRING(REGEX REPLACE ".* ([0-9]+\\.[0-9]+).*" "\\1"
               CPPCheck_VERSION_STRING "${var}")
      ELSE()
        IF(NOT CPPCheck_FIND_QUIETLY)
          MESSAGE(WARNING "regex not supported: ${var}. Please report")
        ENDIF(NOT CPPCheck_FIND_QUIETLY)
      ENDIF()
      STRING(REGEX REPLACE "([0-9]+).*" "\\1" CPPCheck_VERSION_MAJOR "${CPPCheck_VERSION_STRING}")
      STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" CPPCheck_VERSION_MINOR "${CPPCheck_VERSION_STRING}")
      SET(CPPCheck_VERSION ${CPPCheck_VERSION_MAJOR}.${CPPCheck_VERSION_MINOR})
    ENDIF()
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CPPCheck
  REQUIRED_VARS CPPCheck_CPPCHECK_EXECUTABLE
  VERSION_VAR CPPCheck_VERSION
)

MARK_AS_ADVANCED(
  CPPCheck_CPPCHECK_EXECUTABLE
)

