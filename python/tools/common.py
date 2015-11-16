#!/usr/bin/env python3

''' provides common functions for tool searches '''

import shutil
import subprocess


class NotFoundError(Exception):
    ''' thrown when a tool is not found '''

    def __init__(self, name):
        super(NotFoundError, self).__init__()
        self.__name = name

    def name(self):
        return self.__name


class VersionTooOldError(Exception):
    ''' thrown when the version number of a tool is too old '''

    def __init__(self, name, found_version, min_version):
        super(VersionTooOldError, self).__init__()
        self.__name = name
        self.__found_version = found_version
        self.__min_version = min_version

    def name(self):
        return self.__name

    def found_version(self):
        return '.'.join(str(x) for x in self.__found_version)

    def min_version(self):
        return '.'.join(str(x) for x in self.__min_version)


def get_tool_command_output(args, cmd):
    ''' tries to find the tool and returns the output of a specific command '''

    args.log_obj.info('Searching for ' + cmd[0] + '...')
    path = shutil.which(cmd[0])
    if path is None:
        raise NotFoundError(cmd[0])

    cmd[0] = path
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    return cmd[0], process.communicate()


def is_too_old(args, name, found_version, min_version):
    ''' compares the found version with the minimal version and returns false
        when the found one is older than the new one and true in any other
        case '''

    if min_version > found_version:
        return False

    found_version_string = '.'.join(str(x) for x in found_version)
    min_version_string = '.'.join(str(x) for x in min_version)
    args.log_obj.info('Found ' + name + ' ' + found_version_string +
                      ' which matches minimal version constraint (' +
                      min_version_string + ').')
    return True


def compare_versions(args, name, found_version, min_version):
    ''' compares the found version with the minimal version and throw an
        exception when the found one is older than the new one '''

    if is_too_old(args, name, found_version, min_version) is False:
        raise VersionTooOldError(name, found_version, min_version)


def is_tool_available(cmd):
    ''' Returns true, if tool exists and false when it ts not existing '''

    if shutil.which(cmd) is None:
        return False
    else:
        return True
