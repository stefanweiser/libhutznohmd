#!/usr/bin/env python3

''' contains the check step '''

import logger
import os
import paths
import steps.build
import tools.cppcheck
import xml.etree.ElementTree


class CheckStep(object):
    ''' generates reports of all checking tools '''

    def __init__(self):
        self.buildstep = steps.build.BuildStep()
        self.cppchecktool = tools.cppcheck.CppCheckTool()

    def execute(self, args, path):
        self.cppchecktool.find(args, [1, 70])
        self.buildstep.execute(args, path)

        paths.renew_folder(path.reports)

        args.log_obj.info('Run valgrind...')
        args.log_obj.execute(['valgrind', '--leak-check=full', '--xml=yes',
                              '--xml-file=' + os.path.join(path.reports,
                                                           'valgrind.xml'),
                              path.unittest_bin])

        args.log_obj.info('Run unittest...')
        args.log_obj.execute([path.unittest_bin, '--gtest_output=xml:' +
                              os.path.join(path.reports, 'unittest.xml')])

        src_path = os.path.join(path.project, 'src')
        integrationtest_path = os.path.join(src_path, 'integrationtest')
        lib_path = os.path.join(src_path, 'lib')
        unittest_path = os.path.join(src_path, 'unittest')
        gmock_path = os.path.join(path.project, 'gmock')

        # running cppcheck with all the code, to improve 'unused function'
        # warnings
        args.log_obj.info('Run cppcheck...')
        cppcheck_report_filename = os.path.join(path.reports, 'cppcheck.xml')
        cppcheck_report_file = open(cppcheck_report_filename, 'w')
        args.log_obj.execute([self.cppchecktool.path(), '--xml',
                              '--xml-version=2', '--force', '--language=c++',
                              '--platform=unix64', '--enable=all',
                              '--std=c++11', '--suppress=missingIncludeSystem',
                              '-DNDEBUG', '-UBOOST_HAS_TR1_TUPLE',
                              '-UGTEST_CREATE_SHARED_LIBRARY',
                              '-UGTEST_LINKED_AS_SHARED_LIBRARY',
                              '-UGTEST_HAS_STRING_PIECE_', '-U_AIX',
                              '-U_CPPRTTI', '-U_MSC_VER', '-U__SYMBIAN32__',
                              '-U_WIN32_WCE', '-U__APPLE__', '-U__BORLANDC__',
                              '-U__CYGWIN__', '-U__GNUC__', '-U__HP_aCC',
                              '-U__IBMCPP__', '-U__INTEL_COMPILER',
                              '-U__SUNPRO_CC', '-U__SVR4', '-U__clang__',
                              '-U__hpux', '-U_LIBC', '-U__ANDROID__', '-I',
                              lib_path, '-I', gmock_path, '-I', unittest_path,
                              '-I', integrationtest_path, src_path],
                             cppcheck_report_file, logger.STDERR)
        cppcheck_report_file.close()

        CheckStep.__fix_cppcheck_report(cppcheck_report_filename, path.project)
        CheckStep.__run_rats_and_vera(args, src_path, lib_path, path.reports)

        args.log_obj.info('Run All report files were written to ' +
                          path.reports + '.')

    @staticmethod
    def __fix_cppcheck_report(cppcheck_report_filename, project_path):
        ''' remove unwanted coverage data from xml output '''

        tree = xml.etree.ElementTree.ElementTree()
        tree.parse(cppcheck_report_filename)
        errors_node = tree.find('.//errors')
        if errors_node is not None:
            errors = errors_node.findall('error')
            for error in errors:
                location = error.find('location')
                attr = location.attrib['file']
                if attr.startswith(os.path.join(project_path, 'build')) or \
                   attr.startswith(os.path.join(project_path, 'gmock')) or \
                   attr.startswith(os.path.join(project_path,
                                                'src/examples')) or \
                   attr.startswith(os.path.join(project_path,
                                                'src/integrationtest')) or \
                   attr.startswith(os.path.join(project_path, 'src/unittest')):
                    errors_node.remove(error)
            tree.write(cppcheck_report_filename)

    @staticmethod
    def __run_rats_and_vera(args, src_path, lib_path, report_path):
        ''' will produce reports for rats and vera++ '''

        args.log_obj.info('Run rats...')
        rats_report_file = open(os.path.join(report_path, 'rats.xml'), 'w+')
        args.log_obj.execute(['rats', '--xml', '--resultsonly', '-w', '3',
                              src_path], rats_report_file, logger.STDOUT)
        rats_report_file.close()

        args.log_obj.info('Run vera++...')
        vera_cmd_line = ['vera++', '--checkstyle-report',
                         os.path.join(report_path, 'vera++.xml')]
        for dirpath, _, files in os.walk(lib_path):
            for f in files:
                if f.endswith('.cpp') or f.endswith('.hpp'):
                    vera_cmd_line.append(os.path.join(dirpath, f))
        args.log_obj.execute(vera_cmd_line)

    @staticmethod
    def name():
        return 'check'

    @staticmethod
    def help():
        return 'generates reports of all checking tools'
