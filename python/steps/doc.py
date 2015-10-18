#!/usr/bin/env python3

import httpget
import os


class DocStep(object):
    """ Generates the project's doxygen documentation. """

    def execute(self, args, path):
        plantuml_path = os.path.join(path.download, 'plantuml.jar')
        if not os.path.exists(plantuml_path):
            args.log_obj.info('Download plantuml...')
            os.makedirs(path.download, exist_ok=True)
            httpget.http_get(path.plantuml_url, plantuml_path)

        args.log_obj.info('Generate documentation...')
        args.log_obj.execute(['doxygen', os.path.join(path.project,
                                                      'Doxyfile')])

    def name(self):
        return 'doc'

    def help(self):
        return 'generates documentation'
