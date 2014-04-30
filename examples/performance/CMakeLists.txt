include(${PROJECT_DIR}/cmake/toolchain.cmake)

file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_executable(example_performance ${SOURCES})

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(example_performance restsrv)

include_directories("${PROJECT_DIR}/examples/performance")
include_directories("${PROJECT_DIR}/src")
