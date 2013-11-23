cmake_minimum_required(VERSION 2.8)
project(librest)

add_subdirectory(src)
add_subdirectory(unittest)

set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Weiser <stefan.weiser@bluewin.ch>")
 
include(CPack)
