include(${PROJECT_DIR}/cmake/toolchain.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers -Wno-suggest-attribute=format -Wno-suggest-attribute=noreturn")

file(GLOB_RECURSE SOURCES *.cpp *.c)
file(GLOB_RECURSE HEADERS *.hpp *.h)
file(GLOB_RECURSE GMOCK ${GMOCK_DIR}/*.cc ${GMOCK_DIR}/*.h)

add_executable(unittest_rest ${SOURCES} ${HEADERS} ${GMOCK})

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(unittest_rest rest)

include_directories("${PROJECT_DIR}/src")
include_directories("${PROJECT_DIR}/unittest")
include_directories(SYSTEM "${PROJECT_DIR}/gmock")
