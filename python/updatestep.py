#!/usr/bin/env python3

import os


class UpdateStep:
    """ Updates source file lists. """

    def execute(self, args, path):
        def update_single_path(rootpath):
            found_files = []

            for dirpath, dirnames, files in os.walk(rootpath):
                for file in files:
                    if file.endswith(".cpp") or file.endswith(".cc") or \
                    file.endswith(".hpp") or file.endswith(".h"):
                        filepath = os.path.relpath(os.path.join(dirpath, file),
                                                   rootpath)
                        found_files.append(filepath)

            found_files.sort()

            files_txt = open(os.path.join(rootpath, 'files.txt'), 'w')
            files_txt.write('\n'. join(found_files))

        args.log_obj.info('Update source file lists...')
        for dirpath, dirnames, files in os.walk(os.path.join(path.project,
                                                             'src')):
            for file in files:
                if file == 'files.txt':
                    update_single_path(dirpath)

    def name(self):
        return 'update'

    def help(self):
        return 'generates new file lists'
