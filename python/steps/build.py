#!/usr/bin/env python3

import multiprocessing


class BuildStep(object):
    """ Builds the software. """

    def execute(self, args, path):
        args.log_obj.info('Build project...')
        args.log_obj.execute(['make', '-j' + str(multiprocessing.cpu_count()),
                              'install'])

    def name(self):
        return 'build'

    def help(self):
        return 'compiles the target and installs it to the install path'
