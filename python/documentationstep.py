#!/usr/bin/env python3


class DocumentationStep(object):
    """ Generates the project's doxygen documentation. """

    def execute(self, args, path):
        args.log_obj.info('Generate documentation...')
        args.log_obj.execute(['make', 'doc'])

    def name(self):
        return 'doc'

    def help(self):
        return 'generates documentation'
