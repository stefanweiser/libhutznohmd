#!/usr/bin/env python3

''' contains the gcc tool '''

import re
import tools.common


class GccTool(object):
    ''' searches for the gcc tool '''

    def __init__(self):
        self.__name = 'gcc'
        self.__path = ''

    def does_version_match(self, args, min_version):
        ''' tries to find gcc and checks the version '''

        self.__path, out = tools.common.get_tool_command_output(
            args, [self.__name, '--version'])

        # first line of stdout is of interest
        line = out[0].split(b'\n')[0]
        version_string = re.match(b'[^\\d]*([\\d]+\\.[\\d]+\\.[\\d]+)',
                                  line).groups()[0]
        found_version = [int(d) for d in version_string.split(b'.')]
        return tools.common.is_too_old(args, self.__name, found_version,
                                       min_version)

    def path(self):
        return self.__path

    def is_available(self):
        return tools.common.is_tool_available(self.__name)
