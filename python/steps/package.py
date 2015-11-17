#!/usr/bin/env python3

''' contains the package step '''

import os
import steps.test


class PackageStep(object):
    ''' generates packages for source and binaries '''

    def __init__(self):
        self.teststep = steps.test.TestStep()

    def execute(self, args, path):
        self.teststep.execute(args, path)

        args.log_obj.info('Build packages...')

        tar_name = 'libhutznohmd-' + args.library_version + '.tar.gz'
        src_tar_name = 'libhutznohmd_src-' + args.library_version + '.tar.gz'

        args.log_obj.execute(['tar', '--create', '--gzip', '--owner=root',
                              '--group=root', '--preserve-permissions',
                              '--directory', path.install, '--file', tar_name,
                              '.'])
        args.log_obj.execute(['tar', '--create', '--gzip', '--owner=root',
                              '--group=root', '--preserve-permissions',
                              '--exclude=.git', '--exclude=.gitignore',
                              '--exclude=build', '--exclude=' +
                              os.path.join('python', '__pycache__'),
                              '--exclude=' + os.path.join('python', 'steps',
                                                          '__pycache__'),
                              '--exclude=' + os.path.join('python', 'tools',
                                                          '__pycache__'),
                              '--exclude=*.user', '--directory', path.project,
                              '--file', src_tar_name, '.'])

    @staticmethod
    def name():
        return 'package'

    @staticmethod
    def help():
        return 'builds packages'
