#!/usr/bin/env python3

import os


class PackageStep(object):
    """ Generates packages for source and binaries. """

    def execute(self, args, path):
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
                              '--exclude=build', '--exclude=install',
                              '--exclude=' + os.path.join('python',
                                                          '__pycache__'),
                              '--exclude=' + os.path.join('python', 'steps',
                                                          '__pycache__'),
                              '--exclude=*.user', '--directory', path.project,
                              '--file', src_tar_name, '.'])

    def name(self):
        return 'package'

    def help(self):
        return 'builds packages'
