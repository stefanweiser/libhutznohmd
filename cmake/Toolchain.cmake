# This file is part of librestsrv.
# Copyright (C) 2013-2014 Stefan Weiser
#
# The librestsrv project is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3.0 of the License, or (at your
# option) any later version.
#
# The librestsrv project is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.



IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fsigned-char -pipe -pthread -std=c99 -D_POSIX_C_SOURCE=200809L -pedantic -Weverything -Werror -Wno-padded")
SET(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_DEBUG} --coverage -fno-inline -fno-inline-small-functions -fno-default-inline")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fPIC -fsigned-char -pipe -pthread -std=c++11 -pedantic -pedantic-errors -Weverything -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-unused-function -Wno-sign-conversion -Wno-exit-time-destructors -Wno-missing-prototypes")
SET(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} --coverage")

ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fsigned-char -pipe -pthread -std=c99 -D_POSIX_C_SOURCE=200809L -pedantic -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -Werror -Wextra -Wfloat-equal -Wformat=2 -Winline -Wlogical-op -Wnested-externs -Wold-style-definition -Wpointer-arith -Wshadow -Wstrict-aliasing=2 -Wstrict-overflow=1 -Wstrict-prototypes -Wswitch-enum -Wundef -Wunreachable-code")
SET(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_DEBUG} --coverage -fno-inline -fno-inline-small-functions -fno-default-inline")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fPIC -fsigned-char -pipe -pthread -std=c++11 -pedantic -pedantic-errors -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wdisabled-optimization -Werror -Wextra -Wfloat-equal -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wmissing-braces -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-default -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wno-unused-function")
SET(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} --coverage -fno-inline -fno-inline-small-functions -fno-default-inline")

ENDIF()

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
SET(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} --coverage")
