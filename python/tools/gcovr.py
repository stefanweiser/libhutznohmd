#!/usr/bin/env python3

''' contains the gcovr tool '''

import re
import tools.common


class GcovrTool(object):
    ''' searches for the gcovr tool '''

    def __init__(self):
        self.__name = 'gcovr'
        self.__path = ''

    def find(self, args, min_version):
        ''' tries to find gcovr and checks the version '''

        self.__path, out = tools.common.get_tool_command_output(
            args, [self.__name, '--version'])

        # first line of stdout is of interest
        line = out[0].split(b'\n')[0]
        version_string = re.match(b'^gcovr[\\s]+([\\d]+\\.[\\d]+)',
                                  line).groups()[0]
        found_version = [int(d) for d in version_string.split(b'.')]
        tools.common.compare_versions(args, self.__name, found_version,
                                      min_version)

    def path(self):
        return self.__path
