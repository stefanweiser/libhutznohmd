#!/usr/bin/env python3

import steps.build


class TestStep(object):
    """ Runs unit and integration tests. """

    def __init__(self):
        self.buildstep = steps.build.BuildStep()

    def execute(self, args, path):
        self.buildstep.execute(args, path)

        args.log_obj.info('Run tests...')
        args.log_obj.execute([path.unittest_bin])
        args.log_obj.execute([path.integrationtest_bin])

    def name(self):
        return 'test'

    def help(self):
        return 'executes unit and integration tests'
