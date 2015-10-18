#!/usr/bin/env python3

import os
import shutil


class CleanStep(object):
    ''' Cleans the content of the build path. '''

    def execute(self, args, path):
        args.log_obj.info('Clean project...')
        if os.path.exists(path.cmake):
            shutil.rmtree(path.cmake)

    def name(self):
        return 'clean'

    def help(self):
        return 'removes all built output'
