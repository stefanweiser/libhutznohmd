#!/usr/bin/env python3

''' contains the update step '''

import os


class UpdateStep(object):
    ''' updates source file lists '''

    @staticmethod
    def execute(args, path):
        def update_single_path(rootpath):
            ''' updates file list of a single file '''

            found_files = []

            for dirpath, _, files in os.walk(rootpath):
                for f in files:
                    if f.endswith(".cpp") or f.endswith(".cc") or \
                       f.endswith(".hpp") or f.endswith(".h"):
                        filepath = os.path.relpath(os.path.join(dirpath, f),
                                                   rootpath)
                        found_files.append(filepath)

            found_files.sort()

            files_txt = open(os.path.join(rootpath, 'files.txt'), 'w')
            files_txt.write('\n'. join(found_files))

        args.log_obj.info('Update source file lists...')
        for dirpath, _, files in os.walk(os.path.join(path.project, 'src')):
            for f in files:
                if f == 'files.txt':
                    update_single_path(dirpath)

    @staticmethod
    def name():
        return 'update'

    @staticmethod
    def help():
        return 'generates new file lists'
