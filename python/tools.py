#!/usr/bin/env python3

from locale import getdefaultlocale
from os.path import exists, isdir, join, normcase
from os import curdir, defpath, environ, pathsep
from subprocess import call, PIPE, Popen
from sys import platform


class ToolNotAvailableError(Exception):
    def __init__(self, tool_name):
        self.tool_name = tool_name

    def __str__(self):
        return 'ERROR: Unable to find program ' + self.tool_name + '.'


class VersionDoesNotFitError(Exception):
    def __init__(self, tool_name, min_version, found_version):
        self.tool_name = tool_name
        self.min_version = min_version
        self.found_version = found_version

    def __str__(self):
        return 'ERROR: Need at least version ' + min_version + \
               ' of program ' + command + ', but only ' + found_version + \
               ' was found.'


class Tool:
    def __init__(self, executable_name, minimum_version):
        self.executable_name = executable_name
        self.minimum_version = minimum_version.split('.')

    def get_version(self):
        pass

    def check_availability(self):
        if which(self.executable_name) is None:
            raise ToolNotAvailableError(self.executable_name)

    def check_version(self):
        found_version = self.get_version()
        for i, v in enumerate(self.minimum_version):
            if found_version[i] < v:
                VersionDoesNotFitError(self.executable_name,
                                       self.minimum_version,
                                       found_version)
            elif found_version[i] > v:
                break

        print('INFO: Fulfilling need for version ' +
              '.'.join(self.minimum_version) +
              ' of program ' + self.executable_name + ' with version ' +
              '.'.join(found_version) + '.')


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
    p = Popen(command, stdout=PIPE, stderr=PIPE)
    result = p.communicate()
    return [result[0].decode(encoding), result[1].decode(encoding)]


class AStyleTool(Tool):
    def __init__(self, executable_name, minimum_version, options_path):
        Tool.__init__(self, executable_name, minimum_version)
        self.options_path = options_path

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[1].strip().split()[-1].split('.')

    def execute(self, filename):
        call([self.executable_name, '--options=' + self.options_path,
              filename])


class CMakeTool(Tool):
    def __init__(self, executable_name, minimum_version, script_path,
                 build_path, install_path):
        Tool.__init__(self, executable_name, minimum_version)
        self.script_path = script_path
        self.build_path = build_path
        self.install_path = install_path

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[0].strip().split()[-1].split('.')

    def execute(self, target):
        call([self.executable_name, self.script_path,
              '-DCMAKE_INSTALL_PREFIX=' + self.install_path,
              '-DCMAKE_BUILD_TYPE=' + target], cwd=self.build_path)


class CPPCheckTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[0].strip().split()[-1].split('.')


class DoxygenTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[0].strip().split('.')


class DotTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '-V'])
        return s[1].strip().split()[-2].split('.')


class GCCTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[0].split('\n')[0].split()[-1].split('.')


class JavaTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '-version'])
        return s[1].split('\n')[0].split()[-1][1:-1].split('.')


class LCOVTool(Tool):
    def __init__(self, executable_name, html_generator_name, minimum_version,
                 build_path):
        Tool.__init__(self, executable_name, minimum_version)
        self.html_generator_name = html_generator_name
        self.target_path = join(build_path, 'src')
        self.tracefile = join(self.target_path, 'coverage.info')
        self.output_path = join(build_path, 'lcov')

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[0].strip().split()[-1].split('.')

    def do_basic_trace(self):
        call([self.executable_name, '-c', '-i', '-d', self.target_path, '-o',
              self.tracefile + '.base'], cwd=self.build_path)

    def do_test_trace(self):
        call([self.executable_name, '-c', '-d', self.target_path, '-o',
              self.tracefile + '.test'], cwd=self.build_path)

    def generate_coverage(self):
        call([self.executable_name, '-c', '-d', self.target_path, '-o',
              self.tracefile + '.test'])
        call([self.executable_name, '-a', self.tracefile + '.base', '-a',
              self.tracefile + '.test', '-o', self.tracefile])
        call([self.executable_name, '-r', self.tracefile + '.test',
              '/usr/include/', '-o', self.tracefile])
        call([self.html_generator_name, self.tracefile, '--output-directory',
              self.output_path])


class LizardTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[1].strip().split('.')


class MakeTool(Tool):
    def __init__(self, executable_name, minimum_version, build_path):
        Tool.__init__(self, executable_name, minimum_version)
        self.build_path = build_path

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        print(s[0].split('\n')[0].split()[-1].split('.'))
        return s[0].split('\n')[0].split()[-1].split('.')

    def execute(self, args, working_path='.'):
        call([self.executable_name] + args, cwd=working_path)


class PythonTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[1].strip().split()[-1].split('.')


class RPMBuildTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[0].strip().split()[-1].split('.')


class RATSTool(Tool):
    def __init__(self, executable_name, minimum_version, source_paths):
        Tool.__init__(self, executable_name, minimum_version)
        self.source_paths = source_paths

    def get_version(self):
        s = get_command_output([self.executable_name, '-h'])
        return s[0].split('\n')[0].split()[1][1:].split('.')

    def execute(self):
        call([self.executable_name, '--resultsonly', '-w', '3'] +
             self.source_paths)


class ValgrindTool(Tool):
    def __init__(self, executable_name, minimum_version):
        Tool.__init__(self, executable_name, minimum_version)

    def get_version(self):
        s = get_command_output([self.executable_name, '--version'])
        return s[0].strip().split('-')[-1].split('.')

    def execute(self, application):
        call([self.executable_name, application])
