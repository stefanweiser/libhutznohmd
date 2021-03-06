#!/usr/bin/env python3

''' contains the build step '''

import multiprocessing
import steps.bootstrap


class BuildStep(object):
    ''' builds the software '''

    def __init__(self):
        self.bootstrapstep = steps.bootstrap.BootstrapStep()

    def execute(self, args, path):
        self.bootstrapstep.execute(args, path)

        args.log_obj.info('Build project...')
        args.log_obj.execute(['make', '-j' + str(multiprocessing.cpu_count()),
                              'install'], working_dir=path.cmake())

    @staticmethod
    def name():
        return 'build'

    @staticmethod
    def help():
        return 'compiles the target and installs it to the install path'
