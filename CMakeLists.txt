cmake_minimum_required(VERSION 2.8)
project(librestsrv)

get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} REALPATH)
get_filename_component(GMOCK_DIR ${PROJECT_DIR}/gmock REALPATH)

add_subdirectory(src)
add_subdirectory(unittest)
add_subdirectory(integrationtest)
add_subdirectory(examples)

set(CPACK_GENERATOR "DEB;RPM;TBZ2;TGZ")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Weiser <stefan.weiser@bluewin.ch>")
 
include(CPack)



add_custom_target(doc
                  doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation." VERBATIM)

add_custom_target(cppcheck
                  cppcheck --language=c++ --platform=unix64 --enable=all --std=c++11 --force --suppress=unusedFunction --suppress=missingIncludeSystem -I ./src/ ./src/
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "Checking source code." VERBATIM)

add_custom_target(lizard
                  lizard --warnings_only --CCN 10 --sort cyclomatic_complexity ./src/
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "Checking for high cyclomatic complexity." VERBATIM)
