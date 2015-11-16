#!/usr/bin/env python3

''' contains the cppcheck tool '''

import re
import tools.common


class CppCheckTool(object):
    ''' searches for the cppcheck tool '''

    def __init__(self):
        self.__name = 'cppcheck'
        self.__path = ''

    def find(self, args, min_version):
        ''' tries to find cppcheck and checks the version '''

        self.__path, out = tools.common.get_tool_command_output(
            args, [self.__name, '--version'])

        # first line of stdout is of interest
        line = out[0].split(b'\n')[0]
        version_string = re.match(b'^Cppcheck[\\s]+([\\d]+\\.[\\d]+)',
                                  line).groups()[0]
        found_version = [int(d) for d in version_string.split(b'.')]
        tools.common.compare_versions(args, self.__name, found_version,
                                      min_version)

    def path(self):
        return self.__path

    def is_available(self):
        return tools.common.is_tool_available(self.__name)
