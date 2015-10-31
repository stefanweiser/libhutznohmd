#!/usr/bin/env python3

import os
import paths
import steps.build
import xml.etree.ElementTree


class CoverageStep(object):
    ''' Generates reports of all checking tools. '''

    def __init__(self):
        self.buildstep = steps.build.BuildStep()

    def execute(self, args, path):
        args.target = 'coverage'
        self.buildstep.execute(args, path)

        def run_gcovr(output_filename_base, log_obj):
            filename_base = os.path.join(path.coverage, output_filename_base)
            log_obj.execute(['gcovr', '--branches', '--xml', '--output=' +
                             filename_base + '.xml', '--root', path.project,
                             '--verbose'], working_dir=path.cmake)
            log_obj.execute(['gcovr', '--branches', '--html', '--output=' +
                             filename_base + '.html', '--root', path.project,
                             '--verbose'], working_dir=path.cmake)
            log_obj.execute(['gcovr', '--delete', '--branches', '--output=' +
                             filename_base + '.txt', '--root', path.project,
                             '--verbose'], working_dir=path.cmake)

            # Remove unwanted coverage data from xml output and remove module
            # path from paths.
            tree = xml.etree.ElementTree.ElementTree()
            tree.parse(filename_base + '.xml')
            packages_node = tree.find('.//packages')
            if packages_node is not None:
                source_path = os.path.join(path.project, 'src')
                packages = packages_node.findall('package')
                for package in packages:
                    name = package.attrib['name']
                    if name.startswith('build') or \
                       name.startswith('gmock') or \
                       name.startswith('src.examples') or \
                       name.startswith('src.integrationtest') or \
                       name.startswith('src.unittest'):
                        packages_node.remove(package)
                    else:
                        classes = package.findall('.//class')
                        for cl in classes:
                            filename = os.path.join(path.project,
                                                    cl.attrib['filename'])
                            filename = os.path.realpath(filename)
                            filename = os.path.relpath(filename, source_path)
                            cl.attrib['filename'] = filename
                tree.write(filename_base + '.xml')

        args.log_obj.info('Generate coverage information...')

        paths.renew_folder(path.coverage)
        args.log_obj.info('Collect unittest\'s coverage information...')
        args.log_obj.execute([path.unittest_bin])
        run_gcovr('unittest', args.log_obj)

        args.log_obj.info('Collect integrationtest\'s coverage information...')
        args.log_obj.execute([path.integrationtest_bin])
        run_gcovr('integrationtest', args.log_obj)

        args.log_obj.info('Collect overall coverage information...')
        args.log_obj.execute([path.unittest_bin])
        args.log_obj.execute([path.integrationtest_bin])
        run_gcovr('overall', args.log_obj)

    @staticmethod
    def name():
        return 'coverage'

    @staticmethod
    def help():
        return 'generates reports of all checking tools'
