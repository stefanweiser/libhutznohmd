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
    FIND_PACKAGE(Java 1.7 COMPONENTS Runtime)
    FIND_PACKAGE(Wget 1.13)

    IF(Java_Runtime_FOUND AND WGET_FOUND)
        SET(SONAR_RUNNER_JAR "${CMAKE_CURRENT_BINARY_DIR}/sonar-runner.jar")
        IF(NOT EXISTS "${SONAR_RUNNER_JAR}")
            MESSAGE(STATUS "Downloading SonarRunner")
            EXECUTE_PROCESS(COMMAND "${WGET_EXECUTABLE}"
                            "http://repo1.maven.org/maven2/org/codehaus/sonar/runner/sonar-runner-dist/2.4/sonar-runner-dist-2.4.jar"
                            "-O" "${SONAR_RUNNER_JAR}" OUTPUT_QUIET ERROR_QUIET)
            MESSAGE(STATUS "Downloading SonarRunner - done")
        ENDIF()

        ADD_CUSTOM_TARGET(sonar
                          "${Java_JAVA_EXECUTABLE}" -classpath "${SONAR_RUNNER_JAR}"
                          "-Drunner.home=${CMAKE_CURRENT_BINARY_DIR}"
                          "-Dproject.home=${PROJECT_PATH}"
                          "-Dproject.settings=${CMAKE_CURRENT_BINARY_DIR}/sonar.properties"
                          org.sonar.runner.Main WORKING_DIRECTORY
                          "${PROJECT_PATH}" VERBATIM)
    ELSE()
        MESSAGE(WARNING
                "Target sonar not available, because java or wget is missing.")
    ENDIF()
ENDIF()
