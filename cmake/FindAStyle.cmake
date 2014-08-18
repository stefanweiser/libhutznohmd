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



# - Find AStyle
# This module finds if AStyle is installed. This code sets the following
# variables:
#
#  AStyle_ASTYLE_EXECUTABLE = the full path to Doxygen
#  AStyle_VERSION_STRING    = Version of the package found (astyle version), eg. 2.03
#  AStyle_VERSION_MAJOR     = The major version of the package found.
#  AStyle_VERSION_MINOR     = The minor version of the package found.
#  AStyle_VERSION           = This is set to: $major.$minor
#
# The minimum required version of AStyle can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(AStyle 2.03)
#
# For these components the following variables are set:
#
#  AStyle_FOUND             - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(AStyle)

FIND_PROGRAM(AStyle_ASTYLE_EXECUTABLE
  NAMES astyle
  DOC "Artistic Style (http://astyle.sourceforge.net)"
)

IF(AStyle_ASTYLE_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${AStyle_ASTYLE_EXECUTABLE} --version
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(res)
      IF(${AStyle_FIND_REQUIRED})
        MESSAGE(FATAL_ERROR "Error executing astyle --version")
      ELSE()
        MESSAGE(STATUS "Warning, could not run astyle --version")
      ENDIF()
    ELSE()
      IF(var MATCHES ".* Version [0-9]+\\.[0-9]+\\.*")
        STRING(REGEX REPLACE ".* Version ([0-9]+\\.[0-9]+).*" "\\1"
               AStyle_VERSION_STRING "${var}")
      ELSE()
        IF(NOT AStyle_FIND_QUIETLY)
          MESSAGE(WARNING "regex not supported: ${var}. Please report")
        ENDIF(NOT AStyle_FIND_QUIETLY)
      ENDIF()
      STRING(REGEX REPLACE "([0-9]+).*" "\\1" AStyle_VERSION_MAJOR "${AStyle_VERSION_STRING}")
      STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" AStyle_VERSION_MINOR "${AStyle_VERSION_STRING}")
      SET(AStyle_VERSION ${AStyle_VERSION_MAJOR}.${AStyle_VERSION_MINOR})
    ENDIF()
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AStyle
  REQUIRED_VARS AStyle_ASTYLE_EXECUTABLE
  VERSION_VAR AStyle_VERSION
)

MARK_AS_ADVANCED(
  AStyle_ASTYLE_EXECUTABLE
)

