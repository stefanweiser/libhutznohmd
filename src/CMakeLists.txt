include(${PROJECT_DIR}/cmake/toolchain.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Weffc++ -Winline -Wlong-long -Wmissing-field-initializers -Wpadded -Wswitch-enum")

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.hpp)

add_library(rest SHARED ${SOURCES} ${HEADERS})

include_directories("${PROJECT_DIR}/src")

install(TARGETS rest DESTINATION lib)
install(FILES librest.hpp DESTINATION include)
