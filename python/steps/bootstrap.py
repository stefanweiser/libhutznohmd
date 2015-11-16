#!/usr/bin/env python3

''' contains the bootstrap step '''

import os
import tools.clang
import tools.gcc


class CompilerError(Exception):
    ''' thrown when no valid compiler is found '''

    def __init__(self):
        super(CompilerError, self).__init__()


class BootstrapStep(object):
    ''' bootstraps the cmake target '''

    def __init__(self):
        self.clangtool = tools.clang.ClangTool()
        self.gcctool = tools.gcc.GccTool()

    def execute(self, args, path):
        clang_found = self.clangtool.does_version_match(args, [3, 5])
        gcc_found = self.gcctool.does_version_match(args, [4, 8])
        if (clang_found is False) and (gcc_found is False):
            raise CompilerError()

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
