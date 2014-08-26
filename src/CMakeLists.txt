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



INCLUDE(Toolchain)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Weffc++ -Winline")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlong-long")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-field-initializers")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")

FILE(GLOB_RECURSE SOURCES *.cpp *.c)
FILE(GLOB_RECURSE HEADERS *.hpp *.h)

ADD_LIBRARY(restsrv SHARED ${SOURCES} ${HEADERS})
STYLE_FILES(restsrv ${SOURCES} ${HEADERS})
ADD_DEPENDENCIES(restsrv cppcheck)

INCLUDE_DIRECTORIES("${PROJECT_DIR}/src")

INSTALL(TARGETS restsrv DESTINATION lib)
INSTALL(FILES rest.hpp DESTINATION include)
