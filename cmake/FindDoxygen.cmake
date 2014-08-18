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



# - Find Doxygen
# This module finds if Doxygen is installed. This code sets the following
# variables:
#
#  Doxygen_DOXYGEN_EXECUTABLE = the full path to Doxygen
#  Doxygen_DOT_EXECUTABLE     = the full path to Dot
#  Doxygen_VERSION_STRING     = Version of the package found (doxygen version), eg. 1.7.6.1
#  Doxygen_VERSION_MAJOR      = The major version of the package found.
#  Doxygen_VERSION_MINOR      = The minor version of the package found.
#  Doxygen_VERSION_PATCH      = The patch version of the package found.
#  Doxygen_VERSION_TWEAK      = The tweak version of the package found.
#  Doxygen_VERSION            = This is set to: $major.$minor.$patch.$tweak
#
# The minimum required version of Doxygen can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(Doxygen 1.5)
#
# For these components the following variables are set:
#
#  DOXYGEN_FOUND             - TRUE if all components are found.
#
# Example Usages:
#  FIND_PACKAGE(Doxygen)

FIND_PROGRAM(Doxygen_DOXYGEN_EXECUTABLE
  NAMES doxygen
  PATHS
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\doxygen_is1;Inno Setup: App Path]/bin"
    /Applications/Doxygen.app/Contents/Resources
    /Applications/Doxygen.app/Contents/MacOS
  DOC "Doxygen documentation generation tool (http://www.doxygen.org)"
)

IF(Doxygen_DOXYGEN_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${Doxygen_DOXYGEN_EXECUTABLE} --version
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    IF(res)
      IF(${Doxygen_FIND_REQUIRED})
        MESSAGE(FATAL_ERROR "Error executing doxygen --version")
      ELSE()
        MESSAGE(STATUS "Warning, could not run doxygen --version")
      ENDIF()
    ELSE()
      IF(var MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+.*")
        STRING(REGEX REPLACE "([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1"
               Doxygen_VERSION_STRING "${var}")
      ELSE()
        IF(NOT Doxygen_FIND_QUIETLY)
          MESSAGE(WARNING "regex not supported: ${var}. Please report")
        ENDIF(NOT Doxygen_FIND_QUIETLY)
      ENDIF()
      STRING(REGEX REPLACE "([0-9]+).*" "\\1" Doxygen_VERSION_MAJOR "${Doxygen_VERSION_STRING}")
      STRING(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" Doxygen_VERSION_MINOR "${Doxygen_VERSION_STRING}")
      STRING(REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" Doxygen_VERSION_PATCH "${Doxygen_VERSION_STRING}")
      STRING(REGEX REPLACE "[0-9]+\\.[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" Doxygen_VERSION_TWEAK "${Doxygen_VERSION_STRING}")
      SET(Doxygen_VERSION ${Doxygen_VERSION_MAJOR}.${Doxygen_VERSION_MINOR}.${Doxygen_VERSION_PATCH}.${Doxygen_VERSION_TWEAK})
    ENDIF()
ENDIF()

FIND_PROGRAM(Doxygen_DOT_EXECUTABLE NAMES dot)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Doxygen
  REQUIRED_VARS Doxygen_DOXYGEN_EXECUTABLE Doxygen_DOT_EXECUTABLE
  VERSION_VAR Doxygen_VERSION
)

MARK_AS_ADVANCED(
  Doxygen_DOXYGEN_EXECUTABLE
  Doxygen_DOT_EXECUTABLE
)

