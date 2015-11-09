#!/usr/bin/env python3

''' contains the doc step '''

import httpget
import os
import tools.doxygen
import tools.jre


class DocStep(object):
    ''' generates the project's doxygen documentation '''

    def __init__(self):
        self.doxygentool = tools.doxygen.DoxygenTool()
        self.jretool = tools.jre.JreTool()

    def execute(self, args, path):
        self.doxygentool.find(args, [1, 8, 8])
        self.jretool.find(args, [1, 7, 0])
        plantuml_path = os.path.join(path.download, 'plantuml.jar')
        if not os.path.exists(plantuml_path):
            args.log_obj.info('Download plantuml...')
            os.makedirs(path.download, exist_ok=True)
            httpget.http_get(path.plantuml_url, plantuml_path)

        os.makedirs(path.documentation, exist_ok=True)
        args.log_obj.info('Generate documentation...')
        args.log_obj.execute([self.doxygentool.path(),
                              os.path.join(path.project, 'Doxyfile')],
                             working_dir=path.documentation)

    @staticmethod
    def name():
        return 'doc'

    @staticmethod
    def help():
        return 'generates documentation'
