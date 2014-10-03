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
    FIND_PACKAGE(LCOV 1.9)

    FUNCTION(COVERAGE_TARGET TARGET TEST EXCLUDE)
        IF(LCOV_FOUND)
            SET(OUTPUT "${TEST}_cov")

            ADD_CUSTOM_TARGET(${TARGET}
              ${LCOV_LCOV_EXECUTABLE} -d . -z
              COMMAND ${TEST}
              COMMAND ${LCOV_LCOV_EXECUTABLE} -d . -c -o ${OUTPUT}.info
              COMMAND ${LCOV_LCOV_EXECUTABLE} -r ${OUTPUT}.info ${EXCLUDE}
                'gmock/*' '/usr/*' -o ${OUTPUT}.info.cleaned
              COMMAND ${LCOV_GENHTML_EXECUTABLE} -o ${OUTPUT}
                ${OUTPUT}.info.cleaned
              COMMAND ${CMAKE_COMMAND} -E remove ${OUTPUT}.info
                ${OUTPUT}.info.cleaned
              WORKING_DIRECTORY ${BUILD_PATH})
        ELSE()
            MESSAGE(WARNING "Target ${TARGET} not available," +
              " because lcov is missing.")
        ENDIF()
    ENDFUNCTION()
ELSE()
    FUNCTION(COVERAGE_TARGET)
    ENDFUNCTION()
ENDIF()
