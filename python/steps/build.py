#!/usr/bin/env python3

import multiprocessing
import steps.bootstrap
import steps.update


class BuildStep(object):
    """ Builds the software. """

    def __init__(self):
        self.bootstrapstep = steps.bootstrap.BootstrapStep()
        self.updatestep = steps.update.UpdateStep()

    def execute(self, args, path):
        self.bootstrapstep.execute(args, path)
        self.update.execute(args, path)

        args.log_obj.info('Build project...')
        args.log_obj.execute(['make', '-j' + str(multiprocessing.cpu_count()),
                              'install'], working_dir=path.cmake)

    def name(self):
        return 'build'

    def help(self):
        return 'compiles the target and installs it to the install path'
