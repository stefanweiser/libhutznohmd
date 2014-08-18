INCLUDE(Toolchain)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-attribute=format")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-attribute=noreturn")

FILE(GLOB_RECURSE SOURCES *.cpp *.c)
FILE(GLOB_RECURSE HEADERS *.hpp *.h)
FILE(GLOB_RECURSE GMOCK ${GMOCK_DIR}/*.cc ${GMOCK_DIR}/*.h)

ADD_EXECUTABLE(integrationtest_restsrv ${SOURCES} ${HEADERS} ${GMOCK})
STYLE_FILES(integrationtest_restsrv ${SOURCES} ${HEADERS})
ADD_TEST(integrationtest "${CMAKE_CURRENT_BINARY_DIR}/integrationtest_restsrv")

LINK_DIRECTORIES("${CMAKE_BINARY_DIR}/lib")
TARGET_LINK_LIBRARIES(integrationtest_restsrv restsrv)

INCLUDE_DIRECTORIES("${PROJECT_DIR}/src")
INCLUDE_DIRECTORIES("${PROJECT_DIR}/integrationtest")
INCLUDE_DIRECTORIES(SYSTEM "${PROJECT_DIR}/gmock")
