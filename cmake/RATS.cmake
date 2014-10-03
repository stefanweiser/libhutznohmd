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
    FIND_PACKAGE(RATS 2.4)

    IF(RATS_FOUND)
        ADD_CUSTOM_TARGET(rats
                          ${RATS_RATS_EXECUTABLE} --resultsonly -w 3
                              ./examples ./integrationtest ./src ./unittest
                          WORKING_DIRECTORY "${PROJECT_PATH}" VERBATIM)
    ELSE()
        MESSAGE(WARNING "Target rats not available, because rats is missing.")
    ENDIF()
ENDIF()
