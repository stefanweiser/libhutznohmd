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

IF(NOT MINIMAL)
    FIND_PACKAGE(CPPCheck 1.60 REQUIRED)

    ADD_CUSTOM_TARGET(cppcheck
                      ${CPPCheck_CPPCHECK_EXECUTABLE} --quiet --language=c++
                          --platform=unix64 --enable=all --std=c++11 --force
                          --suppress=unusedFunction
                          --suppress=missingIncludeSystem -I ./src/ ./src/
                      WORKING_DIRECTORY "${PROJECT_PATH}" VERBATIM)
ENDIF()
