#!/usr/bin/env python3


class CleanStep:
    """ Cleans the content of the build path """

    def execute(self, args, paths):
        args.log_obj.info('Clean project...')
        args.log_obj.execute(['make', 'clean'])

    def name(self):
        return 'clean'

    def help(self):
        return 'removes all built output'
