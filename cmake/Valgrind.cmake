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

IF(NOT MINIMAL)
    FIND_PACKAGE(Valgrind 3.7)

    IF(VALGRIND_FOUND)
        ADD_CUSTOM_TARGET(valgrind
                          ${Valgrind_VALGRIND_EXECUTABLE} "${BUILD_PATH}/unittest/unittest_restsrv"
                          COMMAND ${Valgrind_VALGRIND_EXECUTABLE} "${BUILD_PATH}/integrationtest/integrationtest_restsrv"
                          WORKING_DIRECTORY "${BUILD_PATH}")
    ELSE()
        MESSAGE(WARNING "Target valgrind not available,"
          " because valgrind is missing.")
    ENDIF()
ENDIF()
