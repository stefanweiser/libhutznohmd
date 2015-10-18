#!/usr/bin/env python3

import multiprocessing
import os
import paths
import xml.etree.ElementTree


class CoverageStep:
    """ Generates reports of all checking tools. """

    def execute(self, args, path):
        def run_gcovr(output_filename_base, log_obj):
            filename_base = os.path.join(path.coverage, output_filename_base)
            log_obj.execute(['gcovr', '--branches', '--xml', '--output=' +
                             filename_base + '.xml', '--root', path.project,
                             '--verbose'])
            log_obj.execute(['gcovr', '--branches', '--html', '--output=' +
                             filename_base + '.html', '--root', path.project,
                             '--verbose'])
            log_obj.execute(['gcovr', '--delete', '--branches', '--output=' +
                             filename_base + '.txt', '--root', path.project,
                             '--verbose'])

            # Remove unwanted coverage data from xml output.
            tree = xml.etree.ElementTree.ElementTree()
            tree.parse(filename_base + '.xml')
            packages_node = tree.find('.//packages')
            if packages_node is not None:
                packages = packages_node.findall('package')
                for package in packages:
                    name = package.attrib['name']
                    if name.startswith('build') or \
                       name.startswith('gmock') or \
                       name.startswith('src.examples') or \
                       name.startswith('src.integrationtest') or \
                       name.startswith('src.unittest'):
                        packages_node.remove(package)
                tree.write(filename_base + '.xml')

        args.log_obj.info('Bootstrap coverage target...')
        args.log_obj.execute(['cmake', os.path.dirname(path.build),
                              '-DCMAKE_INSTALL_PREFIX=' + path.install,
                              '-DCMAKE_BUILD_TYPE=coverage',
                              '-DMINIMAL=' + str(args.minimal),
                              '-DLIBRARY_VERSION=' + args.library_version])
        args.log_obj.info('Build project...')
        args.log_obj.execute(['make', '-j' + str(multiprocessing.cpu_count()),
                              'install'])

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

    def name(self):
        return 'coverage'

    def help(self):
        return 'generates reports of all checking tools'
