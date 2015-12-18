#!/usr/bin/env python3

''' contains the sonar step '''

import evalfile
import compiler
import httpget
import os
import paths
import steps.check
import steps.coverage
import subprocess
import tools.jre


class SonarStep(object):
    ''' updates sonarqube data '''

    def __init__(self):
        self.checkstep = steps.check.CheckStep()
        self.coveragestep = steps.coverage.CoverageStep()
        self.jretool = tools.jre.JreTool()

    def execute(self, args, path):
        self.jretool.find(args, [1, 7, 0])

        os.environ['project_key'] = 'libhutznohmd'
        os.environ['project_name'] = 'libhutznohmd'
        os.environ['project_path'] = path.project()
        os.environ['build_path'] = path.build()
        os.environ['cmake_path'] = path.cmake()
        os.environ['coverage_path'] = os.path.join('build', 'coverage')
        os.environ['reports_path'] = os.path.join('build', 'reports')
        os.environ['version'] = '0.0.1'
        os.environ['include_paths'] = \
            ','.join(compiler.get_include_list(['-std=c++11', '-DNDEBUG']))

        paths.renew_folder(path.cmake())
        paths.renew_folder(path.reports())
        paths.renew_folder(path.coverage())
        self.coveragestep.execute(args, path)
        self.checkstep.execute(args, path)

        args.log_obj.info('Generate sonar configuration...')
        compiler.write_defines(os.path.join(path.cmake(), 'defines.h'),
                               ['-std=c++11', '-DNDEBUG'])
        sonar_property_file = os.path.join('build', 'sonar.properties')
        evalfile.eval_file(os.path.join(path.project(), 'sonar-cxx.template'),
                           sonar_property_file)

        sonar_runner_path = os.path.join(path.download(), 'sonar-runner.jar')
        if not os.path.exists(sonar_runner_path):
            args.log_obj.info('Download sonar-runner...')
            os.makedirs(path.download(), exist_ok=True)
            httpget.http_get(path.sonar_runner_url(), sonar_runner_path)

        args.log_obj.info('Upload informations onto sonar...')
        subprocess.check_call([self.jretool.path(), '-classpath',
                               sonar_runner_path, '-Drunner.home=build',
                               '-Dproject.home=.', '-Dproject.settings=' +
                               sonar_property_file, 'org.sonar.runner.Main'],
                              cwd=path.project())

    @staticmethod
    def name():
        return 'sonar'

    @staticmethod
    def help():
        return 'updates data on sonarqube server'
