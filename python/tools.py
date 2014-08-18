#!/usr/bin/env python3

from locale import getdefaultlocale
from os.path import exists, isdir, join, normcase
from os import curdir, defpath, environ, pathsep
from subprocess import CalledProcessError, check_call, PIPE, Popen, STDOUT
from sys import platform
from termcolor import colorize, GREEN, RED


class ToolNotAvailableError(Exception):
    def __init__(self, executable_name):
        self.executable_name = executable_name

    def __str__(self):
        return colorize('[FAIL]: ' + self.executable_name + ' not found.', RED)


class VersionDoesNotFitError(Exception):
    def __init__(self, executable_name, minimum_version, found_version):
        self.executable_name = executable_name
        self.minimum_version = minimum_version
        self.found_version = found_version

    def __str__(self):
        return colorize('[FAIL]: ' + self.executable_name + ' (>= ' +
                        self.minimum_version + ') == ' + self.found_version +
                        '.', RED)


def versiontuple(v):
    return tuple(map(int, (v.split("."))))


class Tool:
    def __init__(self, executable_name, minimum_version):
        self.executable_name = executable_name
        self.minimum_version = minimum_version

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s.split('\n')[0].split()[-1]

    def check_availability(self):
        if which(self.executable_name) is None:
            raise ToolNotAvailableError(self.executable_name)

    def check_version(self):
        found_version = self.get_version()
        if versiontuple(found_version) < versiontuple(self.minimum_version):
            raise VersionDoesNotFitError(self.executable_name,
                                         self.minimum_version,
                                         found_version)

        return colorize('[ OK ]: ' + self.executable_name + ' (>= ' +
                        self.minimum_version + ') == ' + found_version + '.',
                        GREEN)


def which(cmd):
    path = environ.get("PATH", defpath)
    if not path:
        return None
    path = path.split(pathsep)

    if platform == "win32":
        # The current directory takes precedence on Windows.
        if not curdir in path:
            path.insert(0, curdir)

        # PATHEXT is necessary to check on Windows.
        pathext = environ.get("PATHEXT", "").split(pathsep)

        # See if the given file matches any of the expected path extensions.
        # This will allow us to short circuit when given "python.exe".
        # If it does match, only test that one, otherwise we have to try
        # others.
        if any(cmd.lower().endswith(ext.lower()) for ext in pathext):
            files = [cmd]
        else:
            files = [cmd + ext for ext in pathext]
    else:
        # On other platforms you don't have things like PATHEXT to tell you
        # what file suffixes are executable, so just pass on cmd as-is.
        files = [cmd]

    seen = set()
    for dir in path:
        normdir = normcase(dir)
        if not normdir in seen:
            seen.add(normdir)
            for thefile in files:
                name = join(dir, thefile)
                if exists(name) and not isdir(name):
                    return name
    return None


def get_command_output(command):
    encoding = getdefaultlocale()[1]
    p = Popen(command, stdout=PIPE, stderr=STDOUT)
    result = p.communicate()
    if p.returncode != 0:
        raise CalledProcessError(p.returncode, command)
    return result[0].decode(encoding)


class ValgrindTool(Tool):
    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s.strip().split('-')[-1]

    def execute(self, application):
        check_call([self.executable_name, application])
