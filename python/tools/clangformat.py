#!/usr/bin/env python3

''' contains the clang-format tool '''

import re
import tools.common


class ClangFormatTool(object):
    ''' searches for the clang-format tool '''

    def __init__(self):
        self.__name = 'clang-format'
        self.__path = ''

    def find(self, args, min_version):
        ''' tries to find clang-format and checks the version '''

        self.__path, out = tools.common.get_tool_command_output(
            args, [self.__name, '--version'])

        # first line of stdout is of interest
        line = out[0].split(b'\n')[0]
        version_string = re.match(b'[^\\d]*([\\d]+\\.[\\d]+\\.[\\d]+)',
                                  line).groups()[0]
        found_version = [int(d) for d in version_string.split(b'.')]
        tools.common.compare_versions(args, self.__name, found_version,
                                      min_version)

    def path(self):
        return self.__path

    def is_available(self):
        return tools.common.is_tool_available(self.__name)
