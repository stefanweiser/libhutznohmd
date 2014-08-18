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



# - Find Lizard
# This module finds if Lizard is installed. This code sets the following
# variables:
#
#  Lizard_LIZARD_EXECUTABLE = the full path to Lizard
#  Lizard_VERSION_STRING    = Version of the package found (lizard version), eg. 1.8.4
#  Lizard_VERSION_MAJOR     = The major version of the package found.
#  Lizard_VERSION_MINOR     = The minor version of the package found.
#  Lizard_VERSION_PATCH     = The patch version of the package found.
#  Lizard_VERSION           = This is set to: $major.$minor.$patch
#
# The minimum required version of Lizard can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(Lizard 1.8.4)
#
# For these components the following variables are set:
#
#  Lizard_FOUND             - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(Lizard)

FIND_PROGRAM(Lizard_LIZARD_EXECUTABLE
  NAMES lizard
  DOC "Lizard (https://github.com/terryyin/lizard)"
)

IF(Lizard_LIZARD_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${Lizard_LIZARD_EXECUTABLE} --version
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(res)
      IF(${Lizard_FIND_REQUIRED})
        MESSAGE(FATAL_ERROR "Error executing lizard --version")
      ELSE()
        MESSAGE(STATUS "Warning, could not run lizard --version")
      ENDIF()
    ELSE()
      IF(var MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+\\.*")
        STRING(REGEX REPLACE "([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1"
               Lizard_VERSION_STRING "${var}")
      ELSE()
        IF(NOT Lizard_FIND_QUIETLY)
          MESSAGE(WARNING "regex not supported: ${var}. Please report")
        ENDIF(NOT Lizard_FIND_QUIETLY)
      ENDIF()
      STRING(REGEX REPLACE "([0-9]+).*" "\\1" Lizard_VERSION_MAJOR "${Lizard_VERSION_STRING}")
      STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" Lizard_VERSION_MINOR "${Lizard_VERSION_STRING}")
      STRING(REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" Lizard_VERSION_PATCH "${Lizard_VERSION_STRING}")
      SET(Lizard_VERSION ${Lizard_VERSION_MAJOR}.${Lizard_VERSION_MINOR}.${Lizard_VERSION_PATCH})
    ENDIF()
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Lizard
  REQUIRED_VARS Lizard_LIZARD_EXECUTABLE
  VERSION_VAR Lizard_VERSION
)

MARK_AS_ADVANCED(
  Lizard_LIZARD_EXECUTABLE
)

