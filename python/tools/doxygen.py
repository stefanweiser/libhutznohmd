#!/usr/bin/env python3

''' contains the doxygen tool '''

import tools.common


class DoxygenTool(object):
    ''' searches for the doxygen tool '''

    def __init__(self):
        self.__name = 'doxygen'
        self.__path = ''

    def find(self, args, min_version):
        ''' tries to find doxygen and checks the version '''

        self.__path, out = tools.common.get_tool_command_output(
            args, [self.__name, '--version'])

        # first line of stdout is of interest
        version_list = out[0].split(b'\n')[0].split(b'.')
        found_version = [int(d) for d in version_list]
        tools.common.compare_versions(args, self.__name, found_version,
                                      min_version)

    def path(self):
        return self.__path

    def is_available(self):
        return tools.common.is_tool_available(self.__name)
