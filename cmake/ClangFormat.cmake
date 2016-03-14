# This file is part of libhutznohmd.
# Copyright (C) 2013-2016 Stefan Weiser
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

MACRO(STYLE_FILES TARGET)
    SET(FORMAT_COMMANDS)
    FOREACH(ARG ${ARGN})
        LIST(APPEND FORMAT_COMMANDS COMMAND ${CLANG_FORMAT} -i ${ARG})
    ENDFOREACH()
    ADD_CUSTOM_TARGET(format___${TARGET} ${FORMAT_COMMANDS}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      VERBATIM)
    ADD_DEPENDENCIES(${TARGET} format___${TARGET})
ENDMACRO()
