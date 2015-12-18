#!/usr/bin/env python3

''' path related functionality '''

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
        self._project = project_path
        self._plantuml_url = 'http://sourceforge.net/projects/plantuml/' + \
            'files/plantuml.jar/download'
        self._sonar_runner_url = 'http://repo1.maven.org/maven2/org/' + \
            'codehaus/sonar/runner/sonar-runner-dist/2.4/' + \
            'sonar-runner-dist-2.4.jar'

    def project(self):
        ''' returns the project path '''
        return self._project

    def build(self):
        ''' returns the build path '''
        return os.path.join(self.project(), 'build')

    def cmake(self):
        ''' returns the cmake path '''
        return os.path.join(self.build(), 'cmake')

    def documentation(self):
        ''' returns the documentation path '''
        return os.path.join(self.build(), 'documentation')

    def download(self):
        ''' returns the download path '''
        return os.path.join(self.build(), 'download')

    def install(self):
        ''' returns the install path '''
        return os.path.join(self.build(), 'install')

    def reports(self):
        ''' returns the report path '''
        return os.path.join(self.build(), 'reports')

    def coverage(self):
        ''' returns the coverage path '''
        return os.path.join(self.build(), 'coverage')

    def unittest_bin(self):
        ''' returns the unittest_bin path '''
        return os.path.join(self.cmake(), 'src', 'unittest',
                            'unittest_hutznohmd')

    def integrationtest_bin(self):
        ''' returns the integrationtest_bin path '''
        return os.path.join(self.cmake(), 'src', 'integrationtest',
                            'integrationtest_hutznohmd')

    def plantuml_url(self):
        ''' returns the plantuml_url path '''
        return self._plantuml_url

    def sonar_runner_url(self):
        ''' returns the sonar_runner_url path '''
        return self._sonar_runner_url
