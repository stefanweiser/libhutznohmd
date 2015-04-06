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



# - Find Valgrind
# This module finds if Valgrind is installed. This code sets the following
# variables:
#
#  Valgrind_VALGRIND_EXECUTABLE = the full path to Valgrind
#  Valgrind_VERSION_STRING  = Version of the package found (valgrind version)
#  Valgrind_VERSION_MAJOR   = The major version of the package found.
#  Valgrind_VERSION_MINOR   = The minor version of the package found.
#  Valgrind_VERSION_PATCH   = The patch version of the package found.
#  Valgrind_VERSION         = This is set to: $major.$minor.$patch
#
# The minimum required version of Valgrind can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(Valgrind 3.7.0)
#
# For these components the following variables are set:
#
#  VALGRIND_FOUND             - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(Valgrind)

FIND_PROGRAM(Valgrind_VALGRIND_EXECUTABLE
  NAMES valgrind
  DOC "Valgrind (http://valgrind.org/)"
)

IF(Valgrind_VALGRIND_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${Valgrind_VALGRIND_EXECUTABLE} --version
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(res)
        IF(${Valgrind_FIND_REQUIRED})
            MESSAGE(FATAL_ERROR "Error executing Valgrind --version")
        ELSE()
            MESSAGE(STATUS "Warning, could not run Valgrind --version")
        ENDIF()
    ELSE()
        IF(var MATCHES "valgrind-[0-9]+\\.[0-9]+\\.[0-9]+\\.*")
            STRING(REGEX REPLACE "valgrind-([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1"
                   Valgrind_VERSION_STRING "${var}")
        ELSE()
            IF(NOT Valgrind_FIND_QUIETLY)
                MESSAGE(WARNING "regex not supported: ${var}. Please report")
            ENDIF()
        ENDIF()
        STRING(REGEX REPLACE "([0-9]+).*" "\\1" Valgrind_VERSION_MAJOR
               "${Valgrind_VERSION_STRING}")
        STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" Valgrind_VERSION_MINOR
               "${Valgrind_VERSION_STRING}")
        STRING(REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1"
               Valgrind_VERSION_PATCH "${Valgrind_VERSION_STRING}")
        SET(Valgrind_VERSION ${Valgrind_VERSION_MAJOR}.${Valgrind_VERSION_MINOR})
        SET(Valgrind_VERSION ${Valgrind_VERSION}.${Valgrind_VERSION_PATCH})
    ENDIF()
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Valgrind
  REQUIRED_VARS Valgrind_VALGRIND_EXECUTABLE
  VERSION_VAR Valgrind_VERSION
)

MARK_AS_ADVANCED(
  Valgrind_VALGRIND_EXECUTABLE
)

