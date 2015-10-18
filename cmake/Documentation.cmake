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
    FIND_PACKAGE(Doxygen 1.8.8)
    FIND_PACKAGE(Java 1.7 COMPONENTS Runtime)

    IF(Java_Runtime_FOUND AND DOXYGEN_FOUND)
        ADD_CUSTOM_TARGET(doc
                          "${Doxygen_DOXYGEN_EXECUTABLE}"
                          "${PROJECT_SOURCE_DIR}/Doxyfile"
                          WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                          VERBATIM)
    ELSE()
        MESSAGE(WARNING
                "Target doc not available, because java, wget or doxygen is missing.")
    ENDIF()
ENDIF()
