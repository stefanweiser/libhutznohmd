include(${PROJECT_DIR}/cmake/toolchain.cmake)

file(GLOB_RECURSE SOURCES *.cpp *.c)
file(GLOB_RECURSE HEADERS *.hpp *.h)

add_executable(example_http ${SOURCES} ${HEADERS})
style_files(example_http ${SOURCES} ${HEADERS})

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(example_http restsrv)

include_directories("${PROJECT_DIR}/examples/http")
include_directories("${PROJECT_DIR}/src")
