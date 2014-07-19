include(${PROJECT_DIR}/cmake/toolchain.cmake)

file(GLOB_RECURSE SOURCES *.cpp *.c)
file(GLOB_RECURSE HEADERS *.hpp *.h)
file(GLOB_RECURSE GMOCK ${GMOCK_DIR}/*.cc ${GMOCK_DIR}/*.h)

add_executable(unittest_restsrv ${SOURCES} ${HEADERS} ${GMOCK})

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(unittest_restsrv restsrv)

include_directories("${PROJECT_DIR}/src")
include_directories("${PROJECT_DIR}/unittest")
include_directories(SYSTEM "${PROJECT_DIR}/gmock")
