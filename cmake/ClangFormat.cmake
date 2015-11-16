# This file is part of libhutznohmd.
# Copyright (C) 2013-2015 Stefan Weiser
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
    MACRO(STYLE_FILES TARGET)
        SET(FORMAT_COMMANDS)
        FOREACH(ARG ${ARGN})
            LIST(APPEND FORMAT_COMMANDS COMMAND
                ${ClangFormat_CLANGFORMAT_EXECUTABLE} -i ${ARG})
        ENDFOREACH()
        ADD_CUSTOM_TARGET(format___${TARGET} ${FORMAT_COMMANDS} VERBATIM)
        ADD_DEPENDENCIES(${TARGET} format___${TARGET})
    ENDMACRO()
ELSE()
    MACRO(STYLE_FILES)
    ENDMACRO()
ENDIF()
