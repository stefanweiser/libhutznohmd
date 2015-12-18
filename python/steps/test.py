#!/usr/bin/env python3

''' contains the test step '''

import steps.build


class TestStep(object):
    ''' runs unit and integration tests '''

    def __init__(self):
        self.buildstep = steps.build.BuildStep()

    def execute(self, args, path):
        self.buildstep.execute(args, path)

        args.log_obj.info('Run unittest...')
        args.log_obj.execute([path.unittest_bin()])
        args.log_obj.info('Run integrationtest...')
        args.log_obj.execute([path.integrationtest_bin()])

    @staticmethod
    def name():
        return 'test'

    @staticmethod
    def help():
        return 'executes unit and integration tests'
