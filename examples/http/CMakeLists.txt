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

SETUP_COMPILER_FLAGS()

FILE(GLOB_RECURSE
     FILES
     *.cpp
     *.hpp
     *.c
     *.h)

ADD_EXECUTABLE(example_http
               ${FILES})
STYLE_FILES(example_http
            ${FILES})

TARGET_LINK_LIBRARIES(example_http
                      restsrv)

INCLUDE_DIRECTORIES("${PROJECT_PATH}/src")

INSTALL(TARGETS example_http DESTINATION bin)
