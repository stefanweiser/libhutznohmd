#!/usr/bin/env python3

from multiprocessing import cpu_count


class BuildStep:
    """ Builds the software. """

    def execute(self, args, paths):
        args.log_obj.info('Build project...')
        args.log_obj.execute(['make', '-j' + str(cpu_count()), 'all'])

    def name(self):
        return 'build'

    def help(self):
        return 'compiles the target'
