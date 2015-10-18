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


def execute_bootstrap(args):
    check_call(['cmake',
                os.path.dirname(path.build),
                '-DCMAKE_INSTALL_PREFIX=' + path.install,
                '-DCMAKE_BUILD_TYPE=' + args.target,
                '-DMINIMAL=' + str(args.minimal),
                '-DLIBRARY_VERSION=' + args.library_version],
               cwd=path.build)


def execute_build(args):
    check_call(['make', '-j' + str(cpu_count()), 'all'], cwd=path.build)


def execute_check(args):
    paths.renew_folder(path.reports)

    args.log_obj.info('Compile all...')
    args.log_obj.execute(['make', '-j' + str(cpu_count()), 'all'])

    args.log_obj.info('Run valgrind...')
    args.log_obj.execute(['valgrind', '--leak-check=full', '--xml=yes',
                          '--xml-file=' + os.path.join(path.reports,
                                                       'valgrind.xml'),
                          path.unittest_bin])

    args.log_obj.info('Run unittest...')
    args.log_obj.execute([path.unittest_bin, '--gtest_output=xml:' +
                          os.path.join(path.reports, 'unittest.xml')])

    src_path = os.path.join(path.project, 'src')
    integrationtest_path = os.path.join(src_path, 'integrationtest')
    lib_path = os.path.join(src_path, 'lib')
    unittest_path = os.path.join(src_path, 'unittest')
    gmock_path = os.path.join(path.project, 'gmock')

    # Running cppcheck with all the code, to improve 'unused function'
    # warnings.
    args.log_obj.info('Run cppcheck...')
    cppcheck_report_filename = os.path.join(path.reports, 'cppcheck.xml')
    cppcheck_report_file = open(cppcheck_report_filename, 'w')
    args.log_obj.execute(['cppcheck', '--xml', '--xml-version=2', '--force',
                          '--language=c++', '--platform=unix64',
                          '--enable=all', '--suppress=missingIncludeSystem',
                          '--std=c++11', '-DNDEBUG', '-UBOOST_HAS_TR1_TUPLE',
                          '-UGTEST_CREATE_SHARED_LIBRARY',
                          '-UGTEST_LINKED_AS_SHARED_LIBRARY',
                          '-UGTEST_HAS_STRING_PIECE_', '-U_AIX', '-U_CPPRTTI',
                          '-U_MSC_VER', '-U__SYMBIAN32__', '-U_WIN32_WCE',
                          '-U__APPLE__', '-U__BORLANDC__', '-U__CYGWIN__',
                          '-U__GNUC__', '-U__HP_aCC', '-U__IBMCPP__',
                          '-U__INTEL_COMPILER', '-U__SUNPRO_CC', '-U__SVR4',
                          '-U__clang__', '-U__hpux', '-U_LIBC',
                          '-U__ANDROID__', '-I', lib_path, '-I', gmock_path,
                          '-I', unittest_path, '-I', integrationtest_path,
                          src_path], cppcheck_report_file, logger.STDERR)
    cppcheck_report_file.close()

    # Remove unwanted coverage data from xml output.
    tree = ElementTree()
    tree.parse(cppcheck_report_filename)
    errors_node = tree.find('.//errors')
    if errors_node is not None:
        errors = errors_node.findall('error')
        for error in errors:
            location = error.find('location')
            file = location.attrib['file']
            if file.startswith(os.path.join(path.project, 'build')) or \
               file.startswith(os.path.join(path.project, 'gmock')) or \
               file.startswith(os.path.join(path.project, 'src/examples')) or \
               file.startswith(os.path.join(path.project,
                                            'src/integrationtest')) or \
               file.startswith(os.path.join(path.project, 'src/unittest')):
                errors_node.remove(error)
        tree.write(cppcheck_report_filename)

    args.log_obj.info('Run rats...')
    rats_report_file = open(os.path.join(path.reports, 'rats.xml'), 'w+')
    args.log_obj.execute(['rats', '--xml', '--resultsonly', '-w', '3',
                          src_path], rats_report_file, logger.STDOUT)
    rats_report_file.close()

    args.log_obj.info('Run vera++...')
    vera_cmd_line = ['vera++', '--checkstyle-report',
                     os.path.join(path.reports, 'vera++.xml')]
    for dirpath, dirnames, files in os.walk(lib_path):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.hpp'):
                vera_cmd_line.append(os.path.join(dirpath, file))
    args.log_obj.execute(vera_cmd_line)

    args.log_obj.info('Run All report files were written to ' + path.reports +
                      '.')


def execute_clean(args):
    check_call(['make', 'clean'], cwd=path.build)


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
    execute_bootstrap(args)
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


def execute_install(args):
    check_call(['make', 'install'], cwd=path.build)


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
    execute_bootstrap(args)
    execute_update(args)
    execute_build(args)
    execute_test(args)
    execute_install(args)
    execute_package(args)


if __name__ == "__main__":
    if not os.path.exists(path.build):
        os.makedirs(path.build)

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

    version_file = open(os.path.join(path.project, 'version'), 'r')
    args.library_version = version_file.read().strip()
    version_file.close()

    execute_bootstrap(args)

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
