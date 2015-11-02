#!/usr/bin/env python3

''' contains the bootstrap step '''

import os


class BootstrapStep(object):
    ''' bootstraps the cmake target '''

    @staticmethod
    def execute(args, path):
        args.log_obj.info('Bootstrap project for target ' + args.target +
                          '...')
        os.makedirs(path.cmake, exist_ok=True)
        args.log_obj.execute(['cmake', os.path.join(path.cmake, '..', '..'),
                              '-DCMAKE_INSTALL_PREFIX=' + path.install,
                              '-DCMAKE_BUILD_TYPE=' + args.target,
                              '-DMINIMAL=' + str(args.minimal),
                              '-DLIBRARY_VERSION=' + args.library_version],
                             working_dir=path.cmake)

    @staticmethod
    def name():
        # not intended to be used directly
        assert False
        return ''

    @staticmethod
    def help():
        # not intended to be used directly
        assert False
        return ''
