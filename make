#!/usr/bin/env python3

import os
import sys

# add the python subdirectory to the search path
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                'python'))

from argparse import ArgumentParser
from subprocess import CalledProcessError, check_call
import paths
import logger

import allstep
import buildstep
import checkstep
import cleanstep
import coveragestep
import documentationstep
import packagestep
import sonarstep
import teststep
import updatestep


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

    for step in steps:
        step_group.add_argument(step.name(),
                                action='store_true',
                                help=step.help())

    return parser.parse_args()


if __name__ == "__main__":
    if not os.path.exists(path.build):
        os.makedirs(path.build)

    steps = (allstep.AllStep(), buildstep.BuildStep(), checkstep.CheckStep(),
             cleanstep.CleanStep(), coveragestep.CoverageStep(),
             documentationstep.DocumentationStep(), packagestep.PackageStep(),
             sonarstep.SonarStep(), teststep.TestStep(),
             updatestep.UpdateStep())

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
                step_dict = dict([(x.name(), x) for x in steps])
                for step in args.step:
                    step_dict[step].execute(args, path)

            except CalledProcessError as e:
                log_obj.fail('<' + ' '.join(e.cmd) + '> failed (exit code ' +
                             str(e.returncode) + ').')
