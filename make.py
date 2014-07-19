#!/usr/bin/env python3

import os
import sys

# determine path of the script
script_path = os.path.dirname(os.path.realpath(__file__))
python_script_path = os.path.join(script_path, 'python')

# add the python subdirectory to the search path
sys.path.insert(0, python_script_path)

from argparse import ArgumentParser
from itertools import chain
from multiprocessing import cpu_count
from shutil import rmtree
from subprocess import call
from termcolor import colorize, RED

import tools


class Struct:
    def __init__(self, **args):
        self.__dict__.update(args)


def parse_arguments(steps):
    parser = ArgumentParser()
    parser.add_argument('step',
                        nargs='+')

    mutual_target_group = parser.add_mutually_exclusive_group()
    mutual_target_group.add_argument('-d',
                                     '--debug',
                                     default='debug',
                                     dest='target',
                                     action='store_const',
                                     const='debug',
                                     help='use debug options for the build')
    mutual_target_group.add_argument('-r',
                                     '--release',
                                     dest='target',
                                     action='store_const',
                                     const='release',
                                     help='use release options for the build')

    step_group = parser.add_argument_group(
        'build steps')

    for key in sorted(steps):
        step_group.add_argument(key,
                                action='store_true',
                                help=steps[key].help)

    parser.parse_args()
    return parser.parse_args()


build_path = os.path.join(script_path, 'build')
install_path = os.path.join(script_path, 'install')
html_path = os.path.join(build_path, 'html')

astyle_rc = os.path.join(script_path, 'astyle.rc')
source_paths = [os.path.join(script_path, 'examples'),
                os.path.join(script_path, 'integrationtest'),
                os.path.join(script_path, 'src'),
                os.path.join(script_path, 'unittest')]

unittest_path = os.path.join(build_path,
                             'unittest',
                             'unittest_restsrv')
integrationtest_path = os.path.join(build_path,
                                    'integrationtest',
                                    'integrationtest_restsrv')

astyle = tools.AStyleTool('astyle', '2.03', astyle_rc)
cmake = tools.CMakeTool('cmake', '2.8', script_path, build_path, install_path)
cppcheck = tools.Tool('cppcheck', '1.60')
dot = tools.DotTool('dot', '2.26')
doxygen = tools.Tool('doxygen', '1.7.6')
gcov = tools.Tool('gcov', '4.8')
gpp = tools.Tool('g++', '4.8')
java = tools.JavaTool('java', '1.6')
lcov = tools.LCOVTool('lcov', 'genhtml', '1.9', build_path)
lizard = tools.Tool('lizard', '1.8.4')
make = tools.MakeTool('make', '3.81', build_path)
python3 = tools.Tool('python3', '3.2')
rats = tools.RATSTool('rats', '2.4', source_paths)
rpmbuild = tools.Tool('rpmbuild', '4.9')
valgrind = tools.ValgrindTool('valgrind', '3.7.0')


class IsNotBootstrappedError(Exception):
    def __init__(self):
        print(colorize('[FAIL]: Project is not bootstrapped.', RED))


def check_is_bootstrapped():
    if not os.path.exists(build_path):
        raise IsNotBootstrappedError()


def execute_astyle(args):
    astyle.check_availability()
    astyle.check_version()

    files = (os.walk(p) for p in source_paths)
    for root, dirs, files in chain.from_iterable(files):
        for name in files:
            filename = os.path.join(root, name)
            if (filename.endswith('.h') or filename.endswith('.hpp') or
                    filename.endswith('.c') or filename.endswith('.cpp')):
                astyle.execute(filename)


def execute_bootstrap(args):
    cmake.check_availability()
    cmake.check_version()

    os.makedirs(build_path)
    cmake.execute(args.target)


def execute_build(args):
    gpp.check_availability()
    gpp.check_version()
    check_is_bootstrapped()

    make.execute(['-j' + str(cpu_count()), 'all'], build_path)


def execute_check(args):
    cppcheck.check_availability()
    cppcheck.check_version()

    make.execute(['check'], build_path)


def execute_clean(args):
    if os.path.exists(build_path):
        rmtree(build_path)

    if os.path.exists(install_path):
        rmtree(install_path)


def execute_coverage(args):
    gcov.check_availability()
    gcov.check_version()
    lcov.check_availability()
    lcov.check_version()

    target_path = os.path.join(build_path, 'src')
    tracefile = os.path.join(target_path, 'coverage.info')
    lcov_output_path = os.path.join(build_path, 'lcov')

    exec_clean(args)
    exec_bootstrap(Struct(target='coverage'))
    exec_build(args)

    lcov.do_basic_trace()

    exec_unittest(args)
    exec_integrationtest(args)

    lcov.do_test_trace()
    lcov.generate_coverage()


def execute_doc(args):
    dot.check_availability()
    dot.check_version()
    doxygen.check_availability()
    doxygen.check_version()
    java.check_availability()
    java.check_version()
    check_is_bootstrapped

    plantuml_jar = '/tmp/plantuml.jar'
    if os.path.exists(plantuml_jar) is False:
        call(['wget', 'http://sourceforge.net/projects/plantuml/' +
              'files/plantuml.jar/download', '-O', plantuml_jar])

    call(['java', '-Djava.awt.headless=true', '-jar', plantuml_jar, '-v', '-o',
          html_path, script_path + '/src/**.(c|cpp|h|hpp)'])
    make.execute(['doc'], build_path)


def execute_install(args):
    check_is_bootstrapped()

    make.execute(['install'], build_path)


def execute_integrationtest(args):
    check_is_bootstrapped()

    call([integrationtest_path])


def execute_lizard(args):
    lizard.check_availability()
    lizard.check_version()
    check_is_bootstrapped()

    make.execute(['lizard'], build_path)


def execute_package(args):
    rpmbuild.check_availability()
    rpmbuild.check_version()
    check_is_bootstrapped()

    make.execute(['package'], build_path)


def execute_rats(args):
    rats.check_availability()
    rats.check_version()

    rats.execute()


def execute_unittest(args):
    check_is_bootstrapped()

    call([unittest_path])


def execute_valgrind(args):
    check_is_bootstrapped()

    valgrind.execute(unittest_path)
    valgrind.execute(integrationtest_path)


def execute_test(args):
    execute_unittest(args)
    execute_integrationtest(args)


def execute_all(args):
    execute_clean(args)
    execute_bootstrap(args)
    execute_check(args)
    execute_astyle(args)
    execute_build(args)
    execute_unittest(args)
    execute_integrationtest(args)
    execute_install(args)
    execute_package(args)


if __name__ == "__main__":
    python3.check_availability()
    python3.check_version()

    steps = {
        'all': Struct(fn=execute_all,
                      help='builds all steps to make a package'),
        'astyle': Struct(fn=execute_astyle,
                         help='formats all sources'),
        'bootstrap': Struct(fn=execute_bootstrap,
                            help='bootstraps the build'),
        'build': Struct(fn=execute_build,
                        help='compiles the targets'),
        'check': Struct(fn=execute_check,
                        help='checks all sources'),
        'clean': Struct(fn=execute_clean,
                        help='removes all built output'),
        'coverage': Struct(fn=execute_coverage,
                           help='generates lcov output'),
        'doc': Struct(fn=execute_doc,
                      help='compiles documentation'),
        'install': Struct(fn=execute_install,
                          help='installs the targets'),
        'integrationtest': Struct(fn=execute_integrationtest,
                                  help='executes integration tests'),
        'lizard': Struct(fn=execute_lizard,
                         help='checks for cyclic complexity violations'),
        'package': Struct(fn=execute_package,
                          help='builds packages'),
        'rats': Struct(fn=execute_rats,
                       help='searches for security issues'),
        'test': Struct(fn=execute_test,
                       help='executes unit and integration tests'),
        'unittest': Struct(fn=execute_unittest,
                           help='executes unit tests'),
        'valgrind': Struct(fn=execute_valgrind,
                           help='searches for memory leaks')
    }

    args = parse_arguments(steps)

    try:
        for step in args.step:
            steps[step].fn(args)
    except (tools.ToolNotAvailableError,
            tools.VersionDoesNotFitError,
            IsNotBootstrappedError):
        pass
