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
    FIND_PACKAGE(Doxygen 1.7.6)
    FIND_PACKAGE(Java 1.6)
    FIND_PACKAGE(Wget 1.13)

    IF(JAVA_FOUND AND DOXYGEN_FOUND AND WGET_FOUND)
        SET(PLANTUML_JAR "${CMAKE_CURRENT_BINARY_DIR}/plantuml.jar")
        IF(NOT EXISTS "${PLANTUML_JAR}")
            EXECUTE_PROCESS(COMMAND "${WGET_EXECUTABLE}"
                            "http://sourceforge.net/projects/plantuml/files/plantuml.jar/download"
                            "-O" "${PLANTUML_JAR}")
        ENDIF()

        ADD_CUSTOM_TARGET(doc
                          "${Java_JAVA_EXECUTABLE}" "-Djava.awt.headless=true"
                          "-jar" "${PLANTUML_JAR}" "-v" "-o"
                          "${CMAKE_CURRENT_BINARY_DIR}/html"
                          "${PROJECT_PATH}/src/**.(c|cpp|h|hpp)"
                          COMMAND "${Doxygen_DOXYGEN_EXECUTABLE}"
                          "${PROJECT_PATH}/Doxyfile"
                          WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                          VERBATIM)
    ELSE()
        MESSAGE(WARNING
                "Target doc not available, because java, wget or doxygen is missing.")
    ENDIF()
ENDIF()
