#!/usr/bin/env python3

import evalfile
import checkstep
import compiler
import coveragestep
import httpget
import os
import paths
import subprocess


class SonarStep(object):
    """ Updates sonarqube data. """

    def __init__(self):
        self.checkstep = checkstep.CheckStep()
        self.coveragestep = coveragestep.CoverageStep()

    def execute(self, args, path):
        os.environ['project_key'] = 'libhutznohmd'
        os.environ['project_name'] = 'libhutznohmd'
        os.environ['project_path'] = path.project
        os.environ['build_path'] = path.build
        os.environ['coverage_path'] = os.path.join('build', 'coverage')
        os.environ['reports_path'] = os.path.join('build', 'reports')
        os.environ['version'] = '0.0.1'
        os.environ['include_paths'] = \
            ','.join(compiler.get_cxx11_release_include_list())

        paths.renew_folder(path.build)
        self.coveragestep.execute(args, path)
        self.checkstep.execute(args, path)

        args.log_obj.info('Generate sonar configuration...')
        compiler.write_cxx11_release_defines(os.path.join(path.build,
                                                          'defines.h'))
        sonar_property_file = os.path.join('build', 'sonar.properties')
        evalfile.eval_file(os.path.join(path.project, 'sonar-cxx.template'),
                           sonar_property_file)

        args.log_obj.info('Download sonar-runner...')
        sonar_runner_path = os.path.join(path.build, 'sonar-runner.jar')
        if not os.path.exists(sonar_runner_path):
            httpget.http_get(path.sonar_runner_url, sonar_runner_path)

        args.log_obj.info('Download informations onto sonar...')
        subprocess.check_call(['java', '-classpath', sonar_runner_path,
                               '-Drunner.home=build', '-Dproject.home=.',
                               '-Dproject.settings=' + sonar_property_file,
                               'org.sonar.runner.Main'], cwd=path.project)

    def name(self):
        return 'sonar'

    def help(self):
        return 'u   pdates data on sonarqube server'
