#!/usr/bin/env python3

''' contains the program's main '''

import argparse
import os
import subprocess
import steps
import tools.common


def parse_arguments(configured_steps):
    ''' parses the arguments and returns a structure with those '''

    parser = argparse.ArgumentParser()

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
    for step in configured_steps:
        step_group.add_argument(step.name(),
                                action='store_true',
                                help=step.help())

    # parse command line arguments and return resulting structure
    return parser.parse_args()


def main(path, log_obj):
    ''' program's main function '''

    # collect available steps in a list
    step_list = (steps.build.BuildStep(), steps.check.CheckStep(),
                 steps.clean.CleanStep(), steps.coverage.CoverageStep(),
                 steps.doc.DocStep(), steps.package.PackageStep(),
                 steps.sonar.SonarStep(), steps.test.TestStep())

    # parse command line arguments
    args = parse_arguments(step_list)

    # add logger object to make them available to the steps
    args.log_obj = log_obj

    # read current library version from file and add to arguments
    version_file = open(os.path.join(path.project(), 'version'), 'r')
    args.library_version = version_file.read().strip()
    version_file.close()

    # to show a usable error message, when a command will fail
    try:
        # transform the step list into a dictionary with the step's name as key
        step_dict = dict([(x.name(), x) for x in step_list])

        # iterate through the given steps and run everything
        for step in args.step:
            step_dict[step].execute(args, path)

    except subprocess.CalledProcessError as e:
        args.log_obj.failure('<' + ' '.join(e.cmd) + '> failed (exit code ' +
                             str(e.returncode) + ').')

    except steps.bootstrap.CompilerError as e:
        args.log_obj.failure('No valid compiler found.')

    except tools.common.NotFoundError as e:
        args.log_obj.failure('The tool ' + e.name() + ' was not found.')

    except tools.common.VersionTooOldError as e:
        args.log_obj.failure('Expected at least version ' + e.min_version() +
                             ' of ' + e.name() + ', but found version ' +
                             e.found_version() + '.')
