include(${PROJECT_DIR}/cmake/toolchain.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Weffc++ -Winline -Wlong-long -Wmissing-field-initializers -Wswitch-enum")

file(GLOB_RECURSE SOURCES *.cpp *.c)
file(GLOB_RECURSE HEADERS *.hpp *.h)

add_library(restsrv SHARED ${SOURCES} ${HEADERS})
style_files(restsrv ${SOURCES} ${HEADERS})
add_dependencies(restsrv cppcheck)

include_directories("${PROJECT_DIR}/src")

install(TARGETS restsrv DESTINATION lib)
install(FILES rest.hpp DESTINATION include)
