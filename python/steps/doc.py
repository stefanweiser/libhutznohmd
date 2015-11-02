#!/usr/bin/env python3

''' contains the doc step '''

import httpget
import os


class DocStep(object):
    ''' generates the project's doxygen documentation '''

    @staticmethod
    def execute(args, path):
        plantuml_path = os.path.join(path.download, 'plantuml.jar')
        if not os.path.exists(plantuml_path):
            args.log_obj.info('Download plantuml...')
            os.makedirs(path.download, exist_ok=True)
            httpget.http_get(path.plantuml_url, plantuml_path)

        args.log_obj.info('Generate documentation...')
        args.log_obj.execute(['doxygen', os.path.join(path.project,
                                                      'Doxyfile')])

    @staticmethod
    def name():
        return 'doc'

    @staticmethod
    def help():
        return 'generates documentation'
