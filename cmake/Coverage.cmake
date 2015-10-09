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

IF(NOT MINIMAL)
    FIND_PACKAGE(Gcovr 3.2 REQUIRED)

    SET(COVERAGE_PATH "${CMAKE_BINARY_DIR}/coverage")
    FILE(MAKE_DIRECTORY "${COVERAGE_PATH}")
    ADD_CUSTOM_TARGET(coverage
      unittest_hutznohmd
      COMMAND ${GCOVR_GCOVR_EXECUTABLE} --delete --verbose --keep --xml
        "--output=${COVERAGE_PATH}/unittest.xml" --branches --root
        "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND integrationtest_hutznohmd
      COMMAND ${GCOVR_GCOVR_EXECUTABLE} --delete --verbose --keep --xml
        "--output=${COVERAGE_PATH}/integrationtest.xml" --branches --root
        ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND unittest_hutznohmd
      COMMAND integrationtest_hutznohmd
      COMMAND ${GCOVR_GCOVR_EXECUTABLE} --delete --verbose --keep --xml
        "--output=${COVERAGE_PATH}/overall.xml" --branches --root
        ${CMAKE_CURRENT_SOURCE_DIR}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ENDIF()
