#!/usr/bin/env python3

import os
import shutil


def renew_folder(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)


class Paths(object):
    """ Provides all necessary paths. """

    def __init__(self, project_path):
        """ Initializes all paths """
        self.project = project_path
        self.build = os.path.join(self.project, 'build')
        self.cmake = os.path.join(self.build, 'cmake')
        self.install = os.path.join(self.build, 'install')
        self.reports = os.path.join(self.build, 'reports')
        self.coverage = os.path.join(self.build, 'coverage')
        self.unittest_bin = os.path.join(self.cmake, 'src', 'unittest',
                                         'unittest_hutznohmd')
        self.integrationtest_bin = os.path.join(self.cmake, 'src',
                                                'integrationtest',
                                                'integrationtest_hutznohmd')
        self.sonar_runner_url = 'http://repo1.maven.org/maven2/org/' + \
            'codehaus/sonar/runner/sonar-runner-dist/2.4/' + \
            'sonar-runner-dist-2.4.jar'
