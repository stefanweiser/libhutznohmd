#!/usr/bin/env python3


class CleanStep(object):
    """ Cleans the content of the build path. """

    def execute(self, args, path):
        args.log_obj.info('Clean project...')
        args.log_obj.execute(['make', 'clean'])

    def name(self):
        return 'clean'

    def help(self):
        return 'removes all built output'
