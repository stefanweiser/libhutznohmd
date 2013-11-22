cmake_minimum_required(VERSION 2.8)

project(unittest_rest)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "-fPIC -pipe -pthread -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

file(GLOB_RECURSE SOURCES *.cpp *.cc)
file(GLOB_RECURSE HEADERS *.hpp)

add_executable(unittest_rest ${SOURCES} ${HEADERS})

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(unittest_rest rest)

include_directories("${PROJECT_SOURCE_DIR}/../src")
include_directories("${PROJECT_SOURCE_DIR}")

install(TARGETS unittest_rest DESTINATION bin)
