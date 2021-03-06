#!/usr/bin/env python3

''' contains the clang-format tool '''

import re
import tools.common


class ClangFormatTool(object):
    ''' searches for the clang-format tool '''

    def __init__(self):
        self.__name = ['clang-format', 'clang-format-3.7']
        self.__path = ''
        self.__realname = ''

    def find(self, args, min_version):
        ''' tries to find clang-format and checks the version '''

        if not hasattr(self, '__realname'):
            self.is_available()

        self.__path, out = tools.common.get_tool_command_output(
            args, [self.__realname, '--version'])

        # first line of stdout is of interest
        line = out[0].split(b'\n')[0]
        version_string = re.match(b'[^\\d]*([\\d]+\\.[\\d]+\\.[\\d]+)',
                                  line).groups()[0]
        found_version = [int(d) for d in version_string.split(b'.')]
        tools.common.compare_versions(args, self.__realname, found_version,
                                      min_version)

    def path(self):
        return self.__path

    def is_available(self):
        for name in self.__name:
            if tools.common.is_tool_available(name):
                self.__realname = name
                return True
        return False
