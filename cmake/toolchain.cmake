set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -pipe -pthread -std=c99 -D_POSIX_C_SOURCE=200809L -pedantic -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -Werror -Wextra -Wfloat-equal -Wformat=2 -Winline -Wlogical-op -Wnested-externs -Wold-style-definition -Wpointer-arith -Wshadow -Wstrict-aliasing=2 -Wstrict-overflow=1 -Wstrict-prototypes -Wswitch-enum -Wundef -Wunreachable-code")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_DEBUG} --coverage -fno-inline -fno-inline-small-functions -fno-default-inline")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNUSED=\"__attribute__((unused))\" -fno-exceptions -fPIC -pipe -pthread -std=c++11 -pedantic -pedantic-errors -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wdisabled-optimization -Werror -Wextra -Wfloat-equal -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wmissing-braces -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-default -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage -fno-inline -fno-inline-small-functions -fno-default-inline")

set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
