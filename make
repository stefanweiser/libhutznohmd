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
import re
from shutil import rmtree
from subprocess import CalledProcessError, check_call, PIPE, DEVNULL, Popen
from sys import version, version_info
from termcolor import colorize, RED
from evalfile import eval_file
from compiler import get_cxx11_release_include_list, write_cxx11_release_defines


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


def execute_check(args):
    check_is_bootstrapped()

    os.makedirs(build_path + '/reports')
    check_call(['make', '-j' + str(cpu_count())], cwd=build_path)

    check_call(['valgrind', '--leak-check=full', '--xml=yes',
                '--xml-file=' + build_path + '/reports/valgrind.xml',
                build_path + '/unittest/unittest_hutznohmd'],
               cwd=build_path)

    check_call([build_path + '/unittest/unittest_hutznohmd',
                '--gtest_output=xml:./reports/unittest.xml'], cwd=build_path)

    output_file = open(build_path + '/reports/cppcheck.xml', 'w')
    process = Popen(['cppcheck', '--xml', '--xml-version=2', '--quiet',
                     '--language=c++', '--platform=unix64', '--enable=all',
                     '--std=c++11', '--force', '-I', script_path + '/src',
                     script_path + '/src'], cwd=build_path, stderr=output_file)
    process.wait()
    output_file.close()

    output_file = open(build_path + '/reports/rats.xml', 'w')
    process = Popen(['rats', '--xml', '--resultsonly', '-w', '3',
                     script_path + '/examples',
                     script_path + '/integrationtest', script_path + '/src',
                     script_path + '/unittest'], cwd=build_path,
                    stdout=output_file)
    process.wait()
    output_file.close()


def execute_clean(args):
    check_call(['make', 'clean'], cwd=build_path)


def execute_coverage(args):
    args.target = 'coverage'
    execute_bootstrap(args)
    check_call(['make', '-j' + str(cpu_count()), 'coverage'],
               cwd=build_path)


def execute_doc(args):
    check_is_bootstrapped()

    check_call(['make', 'doc'], cwd=build_path)


def execute_install(args):
    check_is_bootstrapped()

    check_call(['make', 'install'], cwd=build_path)


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


def execute_sonar(args):
    os.environ['project_key'] = 'libhutznohmd'
    os.environ['project_name'] = 'libhutznohmd'
    os.environ['project_path'] = script_path
    os.environ['build_path'] = build_path
    os.environ['coverage_path'] = 'build/coverage'
    os.environ['reports_path'] = 'build/reports'
    os.environ['version'] = '0.0.1'
    os.environ['include_paths'] = ','.join(get_cxx11_release_include_list())

    rmtree(build_path)
    os.makedirs(build_path)
    execute_coverage(args)

    execute_check(args)
    write_cxx11_release_defines(build_path + '/defines.h');
    eval_file(script_path + '/sonar-cxx.template',
              build_path + '/sonar.properties')
    check_call(['make', 'sonar'], cwd=build_path)


def execute_test(args):
    check_is_bootstrapped()

    check_call(['make', 'test'], cwd=build_path)


def update_single_path(rootpath):
    found_files=[]

    for dirpath, dirnames, files in os.walk(rootpath):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".cc") or \
               file.endswith(".hpp") or file.endswith(".h"):
                found_files.append(os.path.relpath(os.path.join(dirpath, file),
                                                   rootpath))

    found_files.sort()

    files_txt = open(os.path.join(rootpath, 'files.txt'), 'w')
    files_txt.write('\n'. join(found_files))


def execute_update(args):
    for dirpath, dirnames, files in os.walk(script_path):
        for file in files:
            if file == 'files.txt':
                update_single_path(dirpath)


def execute_all(args):
    execute_clean(args)
    execute_bootstrap(args)
    execute_update(args)
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
                'check': Struct(fn=execute_check,
                                help='checks code for problems'),
                'clean': Struct(fn=execute_clean,
                                help='removes all built output'),
                'coverage': Struct(fn=execute_coverage,
                                   help='generates lcov output'),
                'doc': Struct(fn=execute_doc,
                              help='compiles documentation'),
                'install': Struct(fn=execute_install,
                                  help='installs the targets'),
                'package': Struct(fn=execute_package,
                                  help='builds packages'),
                'sonar': Struct(fn=execute_sonar,
                                help='uploads sonar results'),
                'test': Struct(fn=execute_test,
                               help='executes unit and integration tests'),
                'update': Struct(fn=execute_update,
                                 help='generates new file lists')
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
