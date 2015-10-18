#!/usr/bin/env python3

import logger
import multiprocessing
import os
import paths
import xml.etree.ElementTree


class CheckStep:
    """ Generates reports of all checking tools. """

    def execute(self, args, path):
        paths.renew_folder(path.reports)

        args.log_obj.info('Compile all...')
        args.log_obj.execute(['make', '-j' + str(multiprocessing.cpu_count()),
                              'install'])

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

        # Running cppcheck with all the code, to improve 'unused function'
        # warnings.
        args.log_obj.info('Run cppcheck...')
        cppcheck_report_filename = os.path.join(path.reports, 'cppcheck.xml')
        cppcheck_report_file = open(cppcheck_report_filename, 'w')
        args.log_obj.execute(['cppcheck', '--xml', '--xml-version=2',
                              '--force', '--language=c++', '--platform=unix64',
                              '--enable=all', '--std=c++11',
                              '--suppress=missingIncludeSystem', '-DNDEBUG',
                              '-UBOOST_HAS_TR1_TUPLE',
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

        # Remove unwanted coverage data from xml output.
        tree = xml.etree.ElementTree.ElementTree()
        tree.parse(cppcheck_report_filename)
        errors_node = tree.find('.//errors')
        if errors_node is not None:
            errors = errors_node.findall('error')
            for error in errors:
                location = error.find('location')
                file = location.attrib['file']
                if file.startswith(os.path.join(path.project, 'build')) or \
                   file.startswith(os.path.join(path.project, 'gmock')) or \
                   file.startswith(os.path.join(path.project,
                                                'src/examples')) or \
                   file.startswith(os.path.join(path.project,
                                                'src/integrationtest')) or \
                   file.startswith(os.path.join(path.project, 'src/unittest')):
                    errors_node.remove(error)
            tree.write(cppcheck_report_filename)

        args.log_obj.info('Run rats...')
        rats_report_file = open(os.path.join(path.reports, 'rats.xml'), 'w+')
        args.log_obj.execute(['rats', '--xml', '--resultsonly', '-w', '3',
                              src_path], rats_report_file, logger.STDOUT)
        rats_report_file.close()

        args.log_obj.info('Run vera++...')
        vera_cmd_line = ['vera++', '--checkstyle-report',
                         os.path.join(path.reports, 'vera++.xml')]
        for dirpath, dirnames, files in os.walk(lib_path):
            for file in files:
                if file.endswith('.cpp') or file.endswith('.hpp'):
                    vera_cmd_line.append(os.path.join(dirpath, file))
        args.log_obj.execute(vera_cmd_line)

        args.log_obj.info('Run All report files were written to ' +
                          path.reports + '.')

    def name(self):
        return 'check'

    def help(self):
        return 'generates reports of all checking tools'
