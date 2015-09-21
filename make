#!/usr/bin/env python3

import os
import sys

# determine path of the script
script_path = os.path.dirname(os.path.realpath(__file__))
python_script_path = os.path.join(script_path, 'python')

# add the python subdirectory to the search path
sys.path.insert(0, python_script_path)

from argparse import ArgumentParser
from multiprocessing import cpu_count
from subprocess import CalledProcessError, check_call, PIPE, Popen
from sys import version, version_info
from termcolor import colorize, RED
from evalfile import eval_file


class Struct:
    def __init__(self, **args):
        self.__dict__.update(args)


def parse_arguments(steps):
    parser = ArgumentParser()
    parser.add_argument('step', nargs='+')

    parser.add_argument('-m',
                        '--minimal',
                        dest='minimal',
                        action='store_true',
                        help='builds a minimal dependency toolchain')

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

    return parser.parse_args()


build_path = os.path.join(script_path, 'build')
install_path = os.path.join(script_path, 'install')


class IsNotBootstrappedError(Exception):
    def __str__(self):
        return colorize('[FAIL]: Project is not bootstrapped.', RED)


def check_is_bootstrapped():
    if not os.path.exists(build_path):
        raise IsNotBootstrappedError()


def execute_bootstrap(args):
    check_call(['cmake',
                script_path,
                '-DCMAKE_INSTALL_PREFIX=' + install_path,
                '-DCMAKE_BUILD_TYPE=' + args.target,
                '-DMINIMAL=' + str(args.minimal),
                '-DLIBRARY_VERSION=' + args.library_version],
               cwd=build_path)


def execute_build(args):
    check_is_bootstrapped()

    check_call(['make', '-j' + str(cpu_count())], cwd=build_path)


def execute_clean(args):
    check_call(['make', 'clean'], cwd=build_path)


def execute_coverage(args):
    execute_clean(args)
    args.target = 'coverage'
    execute_bootstrap(args)
    check_call(['make', '-j' + str(cpu_count()), 'coverage'], cwd=build_path)


def execute_doc(args):
    check_is_bootstrapped()

    check_call(['make', 'doc'], cwd=build_path)


def execute_install(args):
    check_is_bootstrapped()

    check_call(['make', 'install'], cwd=build_path)


def execute_lizard(args):
    check_is_bootstrapped()

    check_call(['make', 'lizard'], cwd=build_path)


def execute_package(args):
    check_is_bootstrapped()

    tar_name = 'libhutznohmd-' + args.library_version + '.tar.gz'
    src_tar_name = 'libhutznohmd_src-' + args.library_version + '.tar.gz'

    check_call(['tar', 'cf', os.path.join(build_path, tar_name),
                '--owner=root', '--group=root', '.'], cwd=install_path)
    check_call(['tar', 'cf', os.path.join(build_path, src_tar_name),
                '--owner=root', '--group=root', '--exclude=./.git',
                '--exclude=./.gitignore', '--exclude=./build',
                '--exclude=./install', '--exclude=./python/__pycache__',
                '--exclude=*.user', '.'], cwd=script_path)


def execute_rats(args):
    check_is_bootstrapped()

    check_call(['make', 'rats'], cwd=build_path)


def execute_sonar(args):
    check_is_bootstrapped()

    os.environ['project_key'] = 'libhutznohmd'
    os.environ['project_name'] = 'libhutznohmd'
    os.environ['project_path'] = script_path
    os.environ['build_path'] = build_path
    os.environ['version'] = '0.0.1'
    os.environ['include_paths'] = '/usr/include/c++/4.8.3,/usr/include/' + \
                                  'c++/4.8.3/x86_64-redhat-linux,/usr/' + \
                                  'include/c++/4.8.3/backward,/usr/lib/' + \
                                  'gcc/x86_64-redhat-linux/4.8.3/include,' + \
                                  '/usr/local/include,/usr/include'

    execute_coverage(args)

    check_call(['./unittest/unittest_hutznohmd',
                '--gtest_output=xml:./unit_test_report.xml'], cwd=build_path)

    define_dump = Popen(['g++', '-dM', '-E', '-xc', os.devnull], cwd=build_path,
                       stdout=PIPE)
    defines = define_dump.communicate()[0]
    defines_file = open(build_path + '/defines.h', 'w')
    defines_file.write(defines)
    defines_file.close()

    eval_file(script_path + '/sonar-cxx.template',
              build_path + '/sonar.properties')
    check_call(['make', 'sonar'], cwd=build_path)


def execute_test(args):
    check_is_bootstrapped()

    check_call(['make', 'test'], cwd=build_path)


def execute_valgrind(args):
    check_is_bootstrapped()

    check_call(['make', 'valgrind'], cwd=build_path)


def execute_all(args):
    execute_clean(args)
    execute_bootstrap(args)
    execute_build(args)
    execute_test(args)
    execute_install(args)
    execute_package(args)


if __name__ == "__main__":
    try:
        if version_info < (3, 2):
            print(colorize('[FAIL]: At least python 3.2 expected,' +
                           ' but found:\n',
                           RED))
            print(colorize('python ' + version, RED))
        else:
            steps = {
                'all': Struct(fn=execute_all,
                              help='builds all steps to make a package'),
                'bootstrap': Struct(fn=execute_bootstrap,
                                    help='bootstraps the build'),
                'build': Struct(fn=execute_build,
                                help='compiles the targets'),
                'clean': Struct(fn=execute_clean,
                                help='removes all built output'),
                'coverage': Struct(fn=execute_coverage,
                                   help='generates lcov output'),
                'doc': Struct(fn=execute_doc,
                              help='compiles documentation'),
                'install': Struct(fn=execute_install,
                                  help='installs the targets'),
                'lizard': Struct(fn=execute_lizard,
                                 help='checks for high cyclic complexity'),
                'package': Struct(fn=execute_package,
                                  help='builds packages'),
                'rats': Struct(fn=execute_rats,
                               help='searches for security issues'),
                'sonar': Struct(fn=execute_sonar,
                                help='uploads sonar results'),
                'test': Struct(fn=execute_test,
                               help='executes unit and integration tests'),
                'valgrind': Struct(fn=execute_valgrind,
                                   help='searches for memory leaks')
            }

            args = parse_arguments(steps)

            version_file = open(os.path.join(script_path, 'version'), 'r')
            args.library_version = version_file.read().strip()
            version_file.close()

            if not os.path.exists(build_path):
                os.makedirs(build_path)
                execute_bootstrap(args)

            for step in args.step:
                steps[step].fn(args)

    except CalledProcessError as e:
        print(colorize('[FAIL]: <' + ' '.join(e.cmd) + '> failed (exit code ' +
                       str(e.returncode) + ').', RED))
