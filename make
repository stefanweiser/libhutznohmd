#!/usr/bin/env python3

import os
import sys

# change directory into project path
os.chdir(os.path.dirname(__file__))

# determine path of the script
project_path = os.path.dirname(os.path.realpath(__file__))
python_script_path = os.path.join(project_path, 'python')
build_path = os.path.join(project_path, 'build')
install_path = os.path.join(project_path, 'install')
reports_path = os.path.join(build_path, 'reports')
coverage_path = os.path.join(build_path, 'coverage')
log_file_path = os.path.join(build_path, 'build.log')
unittest_bin = os.path.join(build_path, 'src', 'unittest',
                            'unittest_hutznohmd')
integrationtest_bin = os.path.join(build_path, 'src', 'integrationtest',
                                   'integrationtest_hutznohmd')
sonar_runner_url = 'http://repo1.maven.org/maven2/org/codehaus/sonar/' + \
    'runner/sonar-runner-dist/2.4/sonar-runner-dist-2.4.jar'

# add the python subdirectory to the search path
sys.path.insert(0, python_script_path)

from argparse import ArgumentParser
from multiprocessing import cpu_count
from shutil import rmtree
from subprocess import CalledProcessError, check_call
from sys import version, version_info
from termcolor import BOLD, colorize, GREEN, RED
from evalfile import eval_file
from httpget import http_get
from proclogger import log_process
import compiler


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


class IsNotBootstrappedError(Exception):
    def __str__(self):
        return colorize('[FAIL]: Project is not bootstrapped.', RED, BOLD)


def check_is_bootstrapped():
    if not os.path.exists(build_path):
        raise IsNotBootstrappedError()


def execute_bootstrap(args):
    check_call(['cmake',
                os.path.dirname(build_path),
                '-DCMAKE_INSTALL_PREFIX=' + install_path,
                '-DCMAKE_BUILD_TYPE=' + args.target,
                '-DMINIMAL=' + str(args.minimal),
                '-DLIBRARY_VERSION=' + args.library_version],
               cwd=build_path)


def execute_build(args):
    check_is_bootstrapped()

    check_call(['make', '-j' + str(cpu_count()), 'all'], cwd=build_path)


def execute_check(args):
    check_is_bootstrapped()

    if not os.path.exists(reports_path):
        os.makedirs(reports_path)
    args.log_file = open(log_file_path, 'w')

    print(colorize('[INFO]: Compile all...', GREEN))
    log_process(['make', '-j' + str(cpu_count()), 'all'], build_path,
                args.log_file, args.log_file)

    print(colorize('[INFO]: Run valgrind...', GREEN))
    log_process(['valgrind', '--leak-check=full', '--xml=yes', '--xml-file=' +
                 os.path.join(reports_path, 'valgrind.xml'), unittest_bin],
                build_path, args.log_file, args.log_file)

    print(colorize('[INFO]: Run unittest...', GREEN))
    log_process([unittest_bin, '--gtest_output=xml:' +
                 os.path.join(reports_path, 'unittest.xml')], build_path,
                args.log_file, args.log_file)

    src_path = os.path.join(project_path, 'src')
    lib_path = os.path.join(src_path, 'lib')

    print(colorize('[INFO]: Run cppcheck...', GREEN))
    cppcheck_report_file = open(os.path.join(reports_path, 'cppcheck.xml'),
                                'w')
    log_process(['cppcheck', '--xml', '--xml-version=2', '--quiet', '--force',
                 '--language=c++', '--platform=unix64', '--enable=all',
                 '--std=c++11', '-I', lib_path, lib_path], build_path,
                args.log_file, cppcheck_report_file)
    cppcheck_report_file.close()

    print(colorize('[INFO]: Run rats...', GREEN))
    rats_report_file = open(os.path.join(reports_path, 'rats.xml'), 'w')
    log_process(['rats', '--xml', '--resultsonly', '-w', '3', src_path],
                build_path, rats_report_file, args.log_file)
    rats_report_file.close()

    args.log_file.close()
    print(colorize('[INFO]: All report files were written to ' + reports_path +
                   '.', GREEN))


def execute_clean(args):
    check_call(['make', 'clean'], cwd=build_path)


def run_gcovr(output_filename_base, log_file):
    filename_base = os.path.join(coverage_path, output_filename_base)
    log_process(['gcovr', '--branches', '--xml', '--output=' + filename_base +
                 '.xml', '--root', project_path, '--verbose'], build_path,
                log_file, log_file)
    log_process(['gcovr', '--branches', '--html', '--output=' + filename_base +
                 '.html', '--root', project_path, '--verbose'], build_path,
                log_file, log_file)
    log_process(['gcovr', '--delete', '--branches', '--output=' +
                 filename_base + '.txt', '--root', project_path, '--verbose'],
                build_path, log_file, log_file)


def execute_coverage(args):
    args.target = 'coverage'
    execute_bootstrap(args)
    execute_build(args)
    if os.path.exists(coverage_path):
        rmtree(coverage_path)
    os.makedirs(coverage_path)

    args.log_file = open(log_file_path, 'w')
    print(colorize('[INFO]: Collect unittest\'s coverage information...',
                   GREEN))
    log_process([unittest_bin], build_path, args.log_file, args.log_file)
    run_gcovr('unittest', args.log_file)

    print(colorize('[INFO]: Collect integrationtest\'s coverage' +
                   ' information...', GREEN))
    log_process([integrationtest_bin], build_path, args.log_file,
                args.log_file)
    run_gcovr('integrationtest', args.log_file)

    print(colorize('[INFO]: Collect overall coverage information...', GREEN))
    log_process([unittest_bin], build_path, args.log_file, args.log_file)
    log_process([integrationtest_bin], build_path, args.log_file,
                args.log_file)
    run_gcovr('overall', args.log_file)
    args.log_file.close()


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

    check_call(['tar', '--create', '--gzip', '--preserve-permissions',
                '--owner=root', '--group=root', '--directory', install_path,
                '--file', tar_name, '.'], cwd=build_path)
    check_call(['tar', '--create', '--gzip', '--preserve-permissions',
                '--owner=root', '--group=root', '--exclude=.git',
                '--exclude=.gitignore', '--exclude=build', '--exclude=install',
                '--exclude=' + os.path.join('python', '__pycache__'),
                '--exclude=*.user', '--directory', project_path, '--file',
                src_tar_name, '.'], cwd=build_path)


def execute_sonar(args):
    os.environ['project_key'] = 'libhutznohmd'
    os.environ['project_name'] = 'libhutznohmd'
    os.environ['project_path'] = project_path
    os.environ['build_path'] = build_path
    os.environ['coverage_path'] = os.path.join('build', 'coverage')
    os.environ['reports_path'] = os.path.join('build', 'reports')
    os.environ['version'] = '0.0.1'
    os.environ['include_paths'] = \
        ','.join(compiler.get_cxx11_release_include_list())

    if os.path.exists(build_path):
        rmtree(build_path)
    os.makedirs(build_path)

    print(colorize('[INFO]: Calculate coverage information...', GREEN))
    execute_coverage(args)

    execute_check(args)

    args.log_file = open(log_file_path, 'w')
    print(colorize('[INFO]: Generate sonar configuration...', GREEN))
    compiler.write_cxx11_release_defines(os.path.join(build_path,
                                                      'defines.h'))
    sonar_property_file = os.path.join('build', 'sonar.properties')
    eval_file(os.path.join(project_path, 'sonar-cxx.template'),
              sonar_property_file)

    print(colorize('[INFO]: Download sonar-runner...', GREEN))
    sonar_runner_path = os.path.join(build_path, 'sonar-runner.jar')
    if not os.path.exists(sonar_runner_path):
        http_get(sonar_runner_url, sonar_runner_path)

    print(colorize('[INFO]: Download informations onto sonar...', GREEN))
    check_call(['java', '-classpath', sonar_runner_path, '-Drunner.home=build',
                '-Dproject.home=.', '-Dproject.settings=' +
                sonar_property_file, 'org.sonar.runner.Main'],
               cwd=project_path)
    args.log_file.close()


def execute_test(args):
    check_is_bootstrapped()

    check_call(['make', 'test'], cwd=build_path)


def update_single_path(rootpath):
    found_files = []

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
    for dirpath, dirnames, files in os.walk(os.path.join(project_path, 'src')):
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

            version_file = open(os.path.join(project_path, 'version'), 'r')
            args.library_version = version_file.read().strip()
            version_file.close()

            if not os.path.exists(build_path):
                os.makedirs(build_path)
                execute_bootstrap(args)

            print(colorize('[INFO]: Writing log to ' + log_file_path, GREEN))
            for step in args.step:
                steps[step].fn(args)

    except CalledProcessError as e:
        print(colorize('[FAIL]: <' + ' '.join(e.cmd) + '> failed (exit code ' +
                       str(e.returncode) + ').', RED))
        args.log_file.close()
