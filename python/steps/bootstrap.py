#!/usr/bin/env python3

import os


class BootstrapStep(object):
    ''' Bootstraps the cmake target. '''

    def execute(self, args, path):
        args.log_obj.info('Bootstrap project for target ' + args.target +
                          '...')
        os.makedirs(path.cmake, exist_ok=True)
        args.log_obj.execute(['cmake', os.path.join(path.cmake, '..', '..'),
                              '-DCMAKE_INSTALL_PREFIX=' + path.install,
                              '-DCMAKE_BUILD_TYPE=' + args.target,
                              '-DMINIMAL=' + str(args.minimal),
                              '-DLIBRARY_VERSION=' + args.library_version],
                             working_dir=path.cmake)

    def name(self):
        # not intended to be used directly
        assert(False)
        return ''

    def help(self):
        # not intended to be used directly
        assert(False)
        return ''
