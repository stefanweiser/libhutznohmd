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

SETUP_COMPILER_FLAGS()
DISABLE_GMOCK_DEPENDENT_WARNINGS()

FILE(GLOB_RECURSE
     FILES
     *.cpp
     *.hpp
     *.c
     *.h)
FILE(GLOB_RECURSE
     GMOCK
     ${GMOCK_PATH}/*.cc
     ${GMOCK_PATH}/*.h)

ADD_EXECUTABLE(unittest_restsrv
               ${FILES}
               ${GMOCK})
STYLE_FILES(unittest_restsrv
            ${FILES})
ADD_TEST(unittest
         "${CMAKE_CURRENT_BINARY_DIR}/unittest_restsrv")

COVERAGE_TARGET(coverage
                unittest_restsrv
                'unittest/*')

SET_SOURCE_FILES_PROPERTIES(${GMOCK}
                            PROPERTIES
                            COMPILE_FLAGS
                            -w)

TARGET_LINK_LIBRARIES(unittest_restsrv
                      restsrv)

INCLUDE_DIRECTORIES("${PROJECT_PATH}/src")
INCLUDE_DIRECTORIES("${PROJECT_PATH}/unittest")

INCLUDE_DIRECTORIES(SYSTEM
                    "${PROJECT_PATH}/gmock")

INSTALL(TARGETS unittest_restsrv DESTINATION bin)
