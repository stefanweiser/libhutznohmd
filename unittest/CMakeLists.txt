cmake_minimum_required(VERSION 2.8)

project(unittest_rest)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "-fPIC -pipe -pthread -std=c++11 -pedantic -pedantic-errors -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wdisabled-optimization -Werror -Wfloat-equal -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wmissing-braces -Wmissing-include-dirs -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-default -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

file(GLOB_RECURSE SOURCES *.cpp *.cc)
file(GLOB_RECURSE HEADERS *.hpp)

add_executable(unittest_rest ${SOURCES} ${HEADERS})

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(unittest_rest rest)

include_directories("${PROJECT_SOURCE_DIR}/../src")
include_directories("${PROJECT_SOURCE_DIR}")
