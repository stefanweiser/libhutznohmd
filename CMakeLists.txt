cmake_minimum_required(VERSION 2.8)
project(librestsrv)

get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" REALPATH)
get_filename_component(GMOCK_DIR "${PROJECT_DIR}/gmock" REALPATH)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(AStyle 2.03 REQUIRED)
find_package(CPPCheck 1.60 REQUIRED)
find_package(Doxygen 1.7.6 REQUIRED)
find_package(Java 1.6 REQUIRED)
FIND_PACKAGE(LCOV 1.9)
find_package(Lizard 1.8.4)
find_package(RATS 2.4)
find_package(Wget 1.13 REQUIRED)

include(Coverage)



set(PLANTUML_JAR "${CMAKE_CURRENT_BINARY_DIR}/plantuml.jar")
if(NOT EXISTS "${PLANTUML_JAR}")
    execute_process(COMMAND ${WGET_EXECUTABLE} http://sourceforge.net/projects/plantuml/files/plantuml.jar/download -O ${PLANTUML_JAR})
endif()

enable_testing()

macro(style_files TARGET)
    set(ASTYLE_COMMANDS)
    foreach(ARG ${ARGN})
        list(APPEND ASTYLE_COMMANDS COMMAND ${AStyle_ASTYLE_EXECUTABLE}
                                    --options=astyle.rc -q ${ARG})
    endforeach()
    add_custom_target(astyle___${TARGET} ${ASTYLE_COMMANDS}
                      WORKING_DIRECTORY "${PROJECT_DIR}" VERBATIM)
    add_dependencies(${TARGET} astyle___${TARGET})
endmacro(style_files)



add_custom_target(cppcheck
                  ${CPPCheck_CPPCHECK_EXECUTABLE} --quiet --language=c++
                      --platform=unix64 --enable=all --std=c++11 --force
                      --suppress=unusedFunction --suppress=missingIncludeSystem
                      -I ./src/ ./src/
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" VERBATIM)

add_custom_target(doc
                  ${Java_JAVA_EXECUTABLE} -Djava.awt.headless=true -jar
                      "${PLANTUML_JAR}" -v -o
                      "${CMAKE_CURRENT_BINARY_DIR}/html"
                      "${CMAKE_CURRENT_SOURCE_DIR}/src/**.(c|cpp|h|hpp)"
                  COMMAND ${Doxygen_DOXYGEN_EXECUTABLE}
                      "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" VERBATIM)

if(LIZARD_FOUND)
    add_custom_target(lizard
                      ${Lizard_LIZARD_EXECUTABLE} --warnings_only --CCN 10
                          --sort cyclomatic_complexity ./src/
                      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" VERBATIM)
endif()

if(RATS_FOUND)
    add_custom_target(rats
                      rats --resultsonly -w 3 ./examples ./integrationtest ./src
                          ./unittest
                      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" VERBATIM)
endif()



add_subdirectory(src)
add_subdirectory(unittest)
add_subdirectory(integrationtest)
add_subdirectory(examples)

set(CPACK_GENERATOR "DEB;RPM;TBZ2;TGZ")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Weiser <stefan.weiser@bluewin.ch>")

include(CPack)

