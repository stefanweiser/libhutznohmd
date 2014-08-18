INCLUDE(Toolchain)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-attribute=format")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-attribute=noreturn")

FILE(GLOB_RECURSE SOURCES *.cpp *.c)
FILE(GLOB_RECURSE HEADERS *.hpp *.h)
FILE(GLOB_RECURSE GMOCK ${GMOCK_DIR}/*.cc ${GMOCK_DIR}/*.h)

ADD_EXECUTABLE(unittest_restsrv ${SOURCES} ${HEADERS} ${GMOCK})
STYLE_FILES(unittest_restsrv ${SOURCES} ${HEADERS})
ADD_TEST(unittest "${CMAKE_CURRENT_BINARY_DIR}/unittest_restsrv")
COVERAGE_TARGET(coverage unittest_restsrv 'unittest/*')

LINK_DIRECTORIES("${CMAKE_BINARY_DIR}/lib")
TARGET_LINK_LIBRARIES(unittest_restsrv restsrv)

INCLUDE_DIRECTORIES("${PROJECT_DIR}/src")
INCLUDE_DIRECTORIES("${PROJECT_DIR}/unittest")
INCLUDE_DIRECTORIES(SYSTEM "${PROJECT_DIR}/gmock")
