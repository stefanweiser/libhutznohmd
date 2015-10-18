#!/usr/bin/env python3


class TestStep:
    """ Runs unit and integration tests. """

    def execute(self, args, path):
        args.log_obj.info('Run tests...')
        args.log_obj.execute(['make', 'test'])

    def name(self):
        return 'test'

    def help(self):
        return 'executes unit and integration tests'
