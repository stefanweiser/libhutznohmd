#!/usr/bin/env python3

''' contains the bootstrap step '''

import os
import tools.clang
import tools.clangformat
import tools.gcc


class CompilerError(Exception):
    ''' thrown when no valid compiler is found '''

    def __init__(self):
        super(CompilerError, self).__init__()


class BootstrapStep(object):
    ''' bootstraps the cmake target '''

    def __init__(self):
        self.clangtool = tools.clang.ClangTool()
        self.clangformattool = tools.clangformat.ClangFormatTool()
        self.gcctool = tools.gcc.GccTool()

    def execute(self, args, path):
        clang_found = self.clangtool.does_version_match(args, [3, 7])
        gcc_found = self.gcctool.does_version_match(args, [4, 8])
        if (clang_found is False) and (gcc_found is False):
            raise CompilerError()

        self.clangformattool.find(args, [3, 7])

        args.log_obj.info('Bootstrap project for target ' + args.target +
                          '...')
        os.makedirs(path.cmake(), exist_ok=True)
        args.log_obj.execute(['cmake', os.path.join(path.cmake(), '..', '..'),
                              '-DCMAKE_INSTALL_PREFIX=' + path.install(),
                              '-DCMAKE_BUILD_TYPE=' + args.target,
                              '-DLIBRARY_VERSION=' + args.library_version,
                              '-DCLANG_FORMAT=' + self.clangformattool.path()],
                             working_dir=path.cmake())

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
