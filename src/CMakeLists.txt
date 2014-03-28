include(${PROJECT_DIR}/cmake/toolchain.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Weffc++ -Winline -Wlong-long -Wmissing-field-initializers -Wswitch-enum")

file(GLOB_RECURSE YACC *.y)
file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.hpp)

find_package(BISON REQUIRED)
bison_target(http ${CMAKE_CURRENT_SOURCE_DIR}/http/parser/http.y ${CMAKE_CURRENT_BINARY_DIR}/http.c)
bison_target(httpdate ${CMAKE_CURRENT_SOURCE_DIR}/http/parser/header/httpdate.y ${CMAKE_CURRENT_BINARY_DIR}/httpdate.c)

add_library(rest SHARED ${SOURCES} ${HEADERS} ${YACC} ${BISON_http_OUTPUTS} ${BISON_httpdate_OUTPUTS})

include_directories("${PROJECT_DIR}/src")

install(TARGETS rest DESTINATION lib)
install(FILES librest.hpp DESTINATION include)
