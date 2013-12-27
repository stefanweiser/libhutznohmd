include(${PROJECT_DIR}/cmake/toolchain.cmake)

file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_executable(example_http ${SOURCES})

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(example_http rest)

include_directories("${PROJECT_DIR}/examples/http")
