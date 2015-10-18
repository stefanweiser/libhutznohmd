#!/usr/bin/env python3

import os
import shutil


def renew_folder(path):
    ''' will clear all contents of a path, by deleting and recreating it '''

    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)


class Paths(object):
    ''' provides all necessary paths '''

    def __init__(self, project_path):
        ''' initializes all paths '''

        self.project = project_path
        self.build = os.path.join(self.project, 'build')
        self.cmake = os.path.join(self.build, 'cmake')
        self.download = os.path.join(self.build, 'download')
        self.install = os.path.join(self.build, 'install')
        self.reports = os.path.join(self.build, 'reports')
        self.coverage = os.path.join(self.build, 'coverage')
        self.unittest_bin = os.path.join(self.cmake, 'src', 'unittest',
                                         'unittest_hutznohmd')
        self.integrationtest_bin = os.path.join(self.cmake, 'src',
                                                'integrationtest',
                                                'integrationtest_hutznohmd')

        self.plantuml_url = 'http://sourceforge.net/projects/plantuml/' + \
            'files/plantuml.jar/download'
        self.sonar_runner_url = 'http://repo1.maven.org/maven2/org/' + \
            'codehaus/sonar/runner/sonar-runner-dist/2.4/' + \
            'sonar-runner-dist-2.4.jar'
