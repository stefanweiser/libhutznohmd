#!/usr/bin/env python3

# include system modules
import argparse
import os
import subprocess
import sys

# add the python subdirectory to the search path
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                'python'))

# include own modules
import logger
import paths
import steps


def parse_arguments(steps):
    ''' parses the arguments and returns a structure with those '''
    parser = argparse.ArgumentParser()

    # possible to just create a package with reduced toolset
    parser.add_argument('-m',
                        '--minimal',
                        dest='minimal',
                        action='store_true',
                        help='builds a minimal dependency toolchain')

    # defines targets, which is either debug or release
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

    # defines several "make steps"
    parser.add_argument('step', nargs='+')

    # add step argument for each
    step_group = parser.add_argument_group('build steps')
    for step in steps:
        step_group.add_argument(step.name(),
                                action='store_true',
                                help=step.help())

    # parse command line arguments and return resulting structure
    return parser.parse_args()


def main(path, log_obj):
    step_list = (steps.all.AllStep(), steps.build.BuildStep(),
                 steps.check.CheckStep(), steps.clean.CleanStep(),
                 steps.coverage.CoverageStep(), steps.doc.DocStep(),
                 steps.package.PackageStep(), steps.sonar.SonarStep(),
                 steps.test.TestStep(), steps.update.UpdateStep())

    args = parse_arguments(step_list)
    args.log_obj = log_obj

    version_file = open(os.path.join(path.project, 'version'), 'r')
    args.library_version = version_file.read().strip()
    version_file.close()

    try:
        step_dict = dict([(x.name(), x) for x in step_list])
        for step in args.step:
            step_dict[step].execute(args, path)

    except subprocess.CalledProcessError as e:
        args.log_obj.fail('<' + ' '.join(e.cmd) + '> failed (exit code ' +
                          str(e.returncode) + ').')


if __name__ == "__main__":
    # first of all generate a structure with all necessary paths
    path = paths.Paths(os.path.dirname(os.path.realpath(__file__)))

    # change into project path
    os.chdir(os.path.dirname(__file__))

    # create build path if not exist
    os.makedirs(path.cmake, exist_ok=True)

    # create a logger
    with logger.Logger(os.path.join(path.build, 'build.log'),
                       path.cmake) as log_obj:
        # the script requires at least python 3.2
        if sys.version_info < (3, 2):
            log_obj.fail('At least python 3.2 expected, but found:\npython ' +
                         sys.version)
        else:
            # call the real main
            main(path, log_obj)
