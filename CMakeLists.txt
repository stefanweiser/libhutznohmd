cmake_minimum_required(VERSION 2.8)
project(librest)

add_subdirectory(src)
add_subdirectory(unittest)

set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Weiser <stefan.weiser@bluewin.ch>")
 
include(CPack)


add_custom_target(doc
                  doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation." VERBATIM)

add_custom_target(check
                  cppcheck --language=c++ --platform=unix64 --enable=all --std=c++11 --force -I ./src/ ./src/
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "Checking source code." VERBATIM)
