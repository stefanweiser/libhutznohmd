#!/usr/bin/env python3

import os
import sys

# add the python subdirectory to the search path
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                'python'))

from argparse import ArgumentParser
from subprocess import CalledProcessError, check_call
from evalfile import eval_file
from httpget import http_get
import compiler
import paths
import logger
from buildstep import BuildStep
from checkstep import CheckStep
from cleanstep import CleanStep
from coveragestep import CoverageStep
from documentationstep import DocumentationStep
from packagestep import PackageStep
from updatestep import UpdateStep


buildstep = BuildStep()
checkstep = CheckStep()
cleanstep = CleanStep()
coveragestep = CoverageStep()
documentationstep = DocumentationStep()
packagestep = PackageStep()
updatestep = UpdateStep()

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


def execute_build(args):
    buildstep.execute(args, path)


def execute_check(args):
    checkstep.execute(args, path)


def execute_clean(args):
    cleanstep.execute(args, path)


def execute_coverage(args):
    coveragestep.execute(args, path)


def execute_doc(args):
    documentationstep.execute(args, path)


def execute_package(args):
    packagestep.execute(args, path)


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


def execute_update(args):
    updatestep.execute(args, path)


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
        buildstep.name(): Struct(fn=execute_build, help=buildstep.help()),
        checkstep.name(): Struct(fn=execute_check, help=checkstep.help()),
        cleanstep.name(): Struct(fn=execute_clean, help=cleanstep.help()),
        coveragestep.name(): Struct(fn=execute_coverage,
                                    help=coveragestep.help()),
        documentationstep.name(): Struct(fn=execute_doc,
                                         help=documentationstep.help()),
        packagestep.name(): Struct(fn=execute_package,
                                   help=packagestep.help()),
        'sonar': Struct(fn=execute_sonar, help='uploads sonar results'),
        'test': Struct(fn=execute_test,
                       help='executes unit and integration tests'),
        updatestep.name(): Struct(fn=execute_update, help=updatestep.help())
    }

    args = parse_arguments(steps)

    version_file = open(os.path.join(path.project, 'version'), 'r')
    args.library_version = version_file.read().strip()
    version_file.close()

    check_call(['cmake',
                os.path.dirname(path.build),
                '-DCMAKE_INSTALL_PREFIX=' + path.install,
                '-DCMAKE_BUILD_TYPE=' + args.target,
                '-DMINIMAL=' + str(args.minimal),
                '-DLIBRARY_VERSION=' + args.library_version],
               cwd=path.build)

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
