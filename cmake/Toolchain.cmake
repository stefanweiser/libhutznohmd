# This file is part of libhutznohmd.
# Copyright (C) 2013-2014 Stefan Weiser
#
# The libhutznohmd project is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3.0 of the License,
# or (at your option) any later version.
#
# The libhutznohmd project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the libhutznohmd project; if not, see
# <http://www.gnu.org/licenses/>.

INCLUDE(CheckToolchain)

IF(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")

    SET(ENABLE_THREADS "-pthread")
    SET(USE_CXX11_FLAG "-std=c++11")
    SET(DISABLE_EXCEPTIONS_FLAG "-fno-exceptions")
    SET(USE_SIGNED_CHAR_FLAG "-fsigned-char")
    SET(ENABLE_DEBUGGING_SYMBOLS "-g")
    SET(DISABLE_OPTIMIZATIONS "-O0")
    SET(ENABLE_OPTIMIZATIONS "-O2 -DNDEBUG")
    SET(ENABLE_COVERAGE_INFOS "--coverage")

    SET(USE_STRICTEST_RULES_FLAG "-pedantic -pedantic-errors -Werror")
    IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        SET(USE_STRICTEST_RULES_FLAG "${USE_STRICTEST_RULES_FLAG} -Weverything")
        SET(USE_STRICTEST_RULES_FLAG "${USE_STRICTEST_RULES_FLAG} -Wno-unused-function -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-sign-conversion -Wno-exit-time-destructors -Wno-missing-prototypes")

        SET(WEAKENING_STRICT_RULES_DUE_TO_GMOCK "-Wno-global-constructors -Wno-weak-vtables")
    ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        SET(USE_STRICTEST_RULES_FLAG "${USE_STRICTEST_RULES_FLAG} -Wextra -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wdisabled-optimization -Wfloat-equal -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wmissing-braces -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-default -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wconversion -Weffc++ -Winline -Wlong-long -Wmissing-field-initializers -Wswitch-enum")
        SET(USE_STRICTEST_RULES_FLAG "${USE_STRICTEST_RULES_FLAG} -Wno-unused-function")

        SET(WEAKENING_STRICT_RULES_DUE_TO_GMOCK "-Wno-missing-field-initializers -Wno-suggest-attribute=format -Wno-suggest-attribute=noreturn -Wno-effc++")
    ENDIF()
ENDIF()

MACRO(SETUP_COMPILER_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_CXX11_FLAG} ${DISABLE_EXCEPTIONS_FLAG}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_SIGNED_CHAR_FLAG} ${USE_STRICTEST_RULES_FLAG}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_THREADS}")

    SET(CMAKE_CXX_FLAGS_DEBUG "${DISABLE_OPTIMIZATIONS} ${ENABLE_DEBUGGING_SYMBOLS}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${ENABLE_OPTIMIZATIONS}")
    SET(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} ${ENABLE_COVERAGE_INFOS}")
    SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${ENABLE_COVERAGE_INFOS}")
    SET(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${ENABLE_COVERAGE_INFOS}")
ENDMACRO()

MACRO(DISABLE_GMOCK_DEPENDENT_WARNINGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WEAKENING_STRICT_RULES_DUE_TO_GMOCK}")
ENDMACRO()
