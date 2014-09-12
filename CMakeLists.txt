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



CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(librestsrv)

GET_FILENAME_COMPONENT(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" REALPATH)
GET_FILENAME_COMPONENT(GMOCK_DIR "${PROJECT_DIR}/gmock" REALPATH)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(ClangFormat 1.7.6 REQUIRED)
FIND_PACKAGE(CPPCheck 1.60 REQUIRED)
FIND_PACKAGE(Doxygen 1.7.6)
FIND_PACKAGE(Java 1.6)
FIND_PACKAGE(LCOV 1.9)
FIND_PACKAGE(Lizard 1.8.4)
FIND_PACKAGE(RATS 2.4)
FIND_PACKAGE(Valgrind 3.7)
FIND_PACKAGE(Wget 1.13)

INCLUDE(Coverage)



ENABLE_TESTING()

MACRO(STYLE_FILES TARGET)
    SET(FORMAT_COMMANDS)
    FOREACH(ARG ${ARGN})
        LIST(APPEND FORMAT_COMMANDS COMMAND
            ${ClangFormat_CLANGFORMAT_EXECUTABLE} -i ${ARG})
    ENDFOREACH()
    ADD_CUSTOM_TARGET(format___${TARGET} ${FORMAT_COMMANDS}
                      WORKING_DIRECTORY "${PROJECT_DIR}" VERBATIM)
    ADD_DEPENDENCIES(${TARGET} format___${TARGET})
ENDMACRO()



ADD_CUSTOM_TARGET(cppcheck
                  ${CPPCheck_CPPCHECK_EXECUTABLE} --quiet --language=c++
                      --platform=unix64 --enable=all --std=c++11 --force
                      --suppress=unusedFunction --suppress=missingIncludeSystem
                      -I ./src/ ./src/
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" VERBATIM)

IF(JAVA_FOUND AND DOXYGEN_FOUND AND WGET_FOUND)
    SET(PLANTUML_JAR "${CMAKE_CURRENT_BINARY_DIR}/plantuml.jar")
    IF(NOT EXISTS "${PLANTUML_JAR}")
        EXECUTE_PROCESS(COMMAND ${WGET_EXECUTABLE}
          http://sourceforge.net/projects/plantuml/files/plantuml.jar/download
          -O "${PLANTUML_JAR}")
    ENDIF()

    ADD_CUSTOM_TARGET(doc
                      ${Java_JAVA_EXECUTABLE} -Djava.awt.headless=true -jar
                          "${PLANTUML_JAR}" -v -o
                          "${CMAKE_CURRENT_BINARY_DIR}/html"
                          "${CMAKE_CURRENT_SOURCE_DIR}/src/**.(c|cpp|h|hpp)"
                      COMMAND ${Doxygen_DOXYGEN_EXECUTABLE}
                          "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
                      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" VERBATIM)
ELSE()
    MESSAGE(WARNING "Target doc not available,"
      " because java, wget or doxygen is missing.")
ENDIF()

IF(LIZARD_FOUND)
    ADD_CUSTOM_TARGET(lizard
                      ${Lizard_LIZARD_EXECUTABLE} --warnings_only --CCN 10
                          --sort cyclomatic_complexity ./src/
                      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" VERBATIM)
ELSE()
    MESSAGE(WARNING "Target lizard not available, because lizard is missing.")
ENDIF()

IF(RATS_FOUND)
    ADD_CUSTOM_TARGET(rats
                      rats --resultsonly -w 3 ./examples ./integrationtest ./src
                          ./unittest
                      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" VERBATIM)
ELSE()
    MESSAGE(WARNING "Target rats not available, because rats is missing.")
ENDIF()

IF(VALGRIND_FOUND)
    ADD_CUSTOM_TARGET(valgrind
                      ${Valgrind_VALGRIND_EXECUTABLE} "${CMAKE_BINARY_DIR}/unittest/unittest_restsrv"
                      COMMAND ${Valgrind_VALGRIND_EXECUTABLE} "${CMAKE_BINARY_DIR}/integrationtest/integrationtest_restsrv"
                      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
ELSE()
    MESSAGE(WARNING "Target valgrind not available,"
      " because valgrind is missing.")
ENDIF()



ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(unittest)
ADD_SUBDIRECTORY(integrationtest)
ADD_SUBDIRECTORY(examples)

SET(CPACK_GENERATOR "DEB;RPM;TBZ2;TGZ")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Weiser <stefan.weiser@bluewin.ch>")

INCLUDE(CPack)

