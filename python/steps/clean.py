#!/usr/bin/env python3

import os
import shutil


class CleanStep(object):
    ''' Cleans the content of the build path. '''

    @staticmethod
    def execute(args, path):
        args.log_obj.info('Clean project...')
        if os.path.exists(path.cmake):
            shutil.rmtree(path.cmake)

    @staticmethod
    def name():
        return 'clean'

    @staticmethod
    def help():
        return 'removes all built output'
