#!/usr/bin/env python3

import os
import sys

# add the python subdirectory to the search path
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                'python'))

from argparse import ArgumentParser
from multiprocessing import cpu_count
from subprocess import CalledProcessError, check_call
from xml.etree.ElementTree import ElementTree
from evalfile import eval_file
from httpget import http_get
import compiler
import paths
import logger
from buildstep import BuildStep
from checkstep import CheckStep
from cleanstep import CleanStep


buildstep = BuildStep()
checkstep = CheckStep()
cleanstep = CleanStep()

# change directory into project path
os.chdir(os.path.dirname(__file__))

# determine paths
path = paths.Paths(os.path.dirname(os.path.realpath(__file__)))


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


def bootstrap(args):
    check_call(['cmake',
                os.path.dirname(path.build),
                '-DCMAKE_INSTALL_PREFIX=' + path.install,
                '-DCMAKE_BUILD_TYPE=' + args.target,
                '-DMINIMAL=' + str(args.minimal),
                '-DLIBRARY_VERSION=' + args.library_version],
               cwd=path.build)


def execute_build(args):
    buildstep.execute(args, path)


def execute_check(args):
    checkstep.execute(args, path)


def execute_clean(args):
    cleanstep.execute(args, path)


def run_gcovr(output_filename_base, log_obj):
    filename_base = os.path.join(path.coverage, output_filename_base)
    log_obj.execute(['gcovr', '--branches', '--xml', '--output=' +
                     filename_base + '.xml', '--root', path.project,
                     '--verbose'])
    log_obj.execute(['gcovr', '--branches', '--html', '--output=' +
                     filename_base + '.html', '--root', path.project,
                     '--verbose'])
    log_obj.execute(['gcovr', '--delete', '--branches', '--output=' +
                     filename_base + '.txt', '--root', path.project,
                     '--verbose'])

    # Remove unwanted coverage data from xml output.
    tree = ElementTree()
    tree.parse(filename_base + '.xml')
    packages_node = tree.find('.//packages')
    if packages_node is not None:
        packages = packages_node.findall('package')
        for package in packages:
            if package.attrib['name'].startswith('build') or \
               package.attrib['name'].startswith('gmock') or \
               package.attrib['name'].startswith('src.examples') or \
               package.attrib['name'].startswith('src.integrationtest') or \
               package.attrib['name'].startswith('src.unittest'):
                packages_node.remove(package)
        tree.write(filename_base + '.xml')


def execute_coverage(args):
    args.target = 'coverage'
    bootstrap(args)
    execute_build(args)

    paths.renew_folder(path.coverage)
    args.log_obj.info('Collect unittest\'s coverage information...')
    args.log_obj.execute([path.unittest_bin])
    run_gcovr('unittest', args.log_obj)

    args.log_obj.info('Collect integrationtest\'s coverage information...')
    args.log_obj.execute([path.integrationtest_bin])
    run_gcovr('integrationtest', args.log_obj)

    args.log_obj.info('Collect overall coverage information...')
    args.log_obj.execute([path.unittest_bin])
    args.log_obj.execute([path.integrationtest_bin])
    run_gcovr('overall', args.log_obj)


def execute_doc(args):
    check_call(['make', 'doc'], cwd=path.build)


def execute_package(args):
    tar_name = 'libhutznohmd-' + args.library_version + '.tar.gz'
    src_tar_name = 'libhutznohmd_src-' + args.library_version + '.tar.gz'

    check_call(['tar', '--create', '--gzip', '--preserve-permissions',
                '--owner=root', '--group=root', '--directory', path.install,
                '--file', tar_name, '.'], cwd=path.build)
    check_call(['tar', '--create', '--gzip', '--preserve-permissions',
                '--owner=root', '--group=root', '--exclude=.git',
                '--exclude=.gitignore', '--exclude=build', '--exclude=install',
                '--exclude=' + os.path.join('python', '__pycache__'),
                '--exclude=*.user', '--directory', path.project, '--file',
                src_tar_name, '.'], cwd=path.build)


def execute_sonar(args):
    os.environ['project_key'] = 'libhutznohmd'
    os.environ['project_name'] = 'libhutznohmd'
    os.environ['project_path'] = path.project
    os.environ['build_path'] = path.build
    os.environ['coverage_path'] = os.path.join('build', 'coverage')
    os.environ['reports_path'] = os.path.join('build', 'reports')
    os.environ['version'] = '0.0.1'
    os.environ['include_paths'] = \
        ','.join(compiler.get_cxx11_release_include_list())

    paths.renew_folder(path.build)
    args.log_obj.info('Calculate coverage information...')
    execute_coverage(args)

    execute_check(args)

    args.log_obj.info('Generate sonar configuration...')
    compiler.write_cxx11_release_defines(os.path.join(path.build,
                                                      'defines.h'))
    sonar_property_file = os.path.join('build', 'sonar.properties')
    eval_file(os.path.join(path.project, 'sonar-cxx.template'),
              sonar_property_file)

    args.log_obj.info('Download sonar-runner...')
    sonar_runner_path = os.path.join(path.build, 'sonar-runner.jar')
    if not os.path.exists(sonar_runner_path):
        http_get(path.sonar_runner_url, sonar_runner_path)

    args.log_obj.info('Download informations onto sonar...')
    check_call(['java', '-classpath', sonar_runner_path, '-Drunner.home=build',
                '-Dproject.home=.', '-Dproject.settings=' +
                sonar_property_file, 'org.sonar.runner.Main'],
               cwd=path.project)


def execute_test(args):
    check_call(['make', 'test'], cwd=path.build)


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
    for dirpath, dirnames, files in os.walk(os.path.join(path.project, 'src')):
        for file in files:
            if file == 'files.txt':
                update_single_path(dirpath)


def execute_all(args):
    execute_clean(args)
    execute_update(args)
    execute_build(args)
    execute_test(args)
    execute_package(args)


if __name__ == "__main__":
    if not os.path.exists(path.build):
        os.makedirs(path.build)

    steps = {
        'all': Struct(fn=execute_all,
                      help='builds all steps to make a package'),
        'coverage': Struct(fn=execute_coverage,
                           help='generates lcov output'),
        buildstep.name(): Struct(fn=execute_build, help=buildstep.help()),
        checkstep.name(): Struct(fn=execute_check, help=checkstep.help()),
        cleanstep.name(): Struct(fn=execute_clean, help=cleanstep.help()),
        'doc': Struct(fn=execute_doc, help='compiles documentation'),
        'package': Struct(fn=execute_package, help='builds packages'),
        'sonar': Struct(fn=execute_sonar, help='uploads sonar results'),
        'test': Struct(fn=execute_test,
                       help='executes unit and integration tests'),
        'update': Struct(fn=execute_update, help='generates new file lists')
    }

    args = parse_arguments(steps)

    version_file = open(os.path.join(path.project, 'version'), 'r')
    args.library_version = version_file.read().strip()
    version_file.close()

    bootstrap(args)

    with logger.Logger('build.log', path.build) as log_obj:
        if sys.version_info < (3, 2):
            log_obj.fail('At least python 3.2 expected, but found:\npython ' +
                         sys.version)
        else:
            try:
                args.log_obj = log_obj
                for step in args.step:
                    steps[step].fn(args)

            except CalledProcessError as e:
                log_obj.fail('<' + ' '.join(e.cmd) + '> failed (exit code ' +
                             str(e.returncode) + ').')
