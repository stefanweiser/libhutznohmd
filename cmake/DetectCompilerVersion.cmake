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

FUNCTION(CHECK_GCC_VERSION PATH MIN_VERSION)
    EXECUTE_PROCESS(COMMAND ${PATH} --version
                    RESULT_VARIABLE RES
                    OUTPUT_VARIABLE VAR
                    ERROR_VARIABLE VAR
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_STRIP_TRAILING_WHITESPACE)

    IF(VAR MATCHES ".*[0-9]+\\.[0-9]+\\.[0-9]+.*")
        STRING(REGEX REPLACE ".*([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1" VERSION
               "${VAR}")
        IF(VERSION VERSION_LESS MIN_VERSION)
            MESSAGE(FATAL_ERROR
                    "Found older version than ${MIN_VERSION} of ${PATH} (${VERSION}).")
        ELSE()
            MESSAGE(STATUS
                    "Found newer version than ${MIN_VERSION} of ${PATH} (${VERSION}).")
        ENDIF()
    ELSE()
        MESSAGE(FATAL_ERROR "Failed to find version of ${PATH}.")
    ENDIF()
ENDFUNCTION()

FUNCTION(CHECK_CLANG_VERSION PATH MIN_VERSION)
    EXECUTE_PROCESS(COMMAND "${PATH}" "--version"
                    RESULT_VARIABLE RES
                    OUTPUT_VARIABLE VAR
                    ERROR_VARIABLE VAR
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_STRIP_TRAILING_WHITESPACE)

    IF(VAR MATCHES ".*[0-9]+\\.[0-9]+\\.[0-9]+.*")
        STRING(REGEX REPLACE ".*([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1" VERSION
               "${VAR}")
        IF(VERSION VERSION_LESS MIN_VERSION)
            MESSAGE(FATAL_ERROR
                    "Found older version than ${MIN_VERSION} of ${PATH} (${VERSION}).")
        ELSE()
            MESSAGE(STATUS
                    "Found newer version than ${MIN_VERSION} of ${PATH} (${VERSION}).")
        ENDIF()
    ELSE()
        MESSAGE(FATAL_ERROR "Failed to find version of ${PATH}.")
    ENDIF()
ENDFUNCTION()
