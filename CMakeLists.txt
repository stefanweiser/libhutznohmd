cmake_minimum_required(VERSION 2.8)
project(librestsrv)

get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} REALPATH)
get_filename_component(GMOCK_DIR ${PROJECT_DIR}/gmock REALPATH)



macro(style_files TARGET)
    set(ASTYLE_COMMANDS)
    foreach(ARG ${ARGN})
        list(APPEND ASTYLE_COMMANDS COMMAND /usr/bin/astyle --options=${PROJECT_DIR}/astyle.rc -q ${ARG})
    endforeach()
    add_custom_target(astyle___${TARGET} ${ASTYLE_COMMANDS} VERBATIM)
    add_dependencies(${TARGET} astyle___${TARGET})
endmacro(style_files)

add_custom_target(cppcheck
                  cppcheck --quiet --language=c++ --platform=unix64
                      --enable=all --std=c++11 --force
                      --suppress=unusedFunction --suppress=missingIncludeSystem
                      -I ./src/ ./src/
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} VERBATIM)

add_custom_target(doc
                  doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  VERBATIM)

add_custom_target(lizard
                  lizard --warnings_only --CCN 10 --sort cyclomatic_complexity
                      ./src/
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  VERBATIM)



add_subdirectory(src)
add_subdirectory(unittest)
add_subdirectory(integrationtest)
add_subdirectory(examples)

set(CPACK_GENERATOR "DEB;RPM;TBZ2;TGZ")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Weiser <stefan.weiser@bluewin.ch>")
 
include(CPack)
