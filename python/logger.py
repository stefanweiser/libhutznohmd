#!/usr/bin/env python3

import subprocess
import termcolor
import sys


NONE, STDOUT, STDERR, BOTH = range(4)


class Logger(object):
    """ Provides functions to print out data to log file. """

    def __init__(self, logfile_path):
        """ Sets the path to the log file, but actually does not open it. """
        self.logfile_path = logfile_path
        print(termcolor.colorize('[INFO]: Write log to ' + self.logfile_path,
                                 termcolor.GREEN))

    def __enter__(self):
        """ Opens the log file. """
        print(termcolor.colorize('[INFO]: Open log file.', termcolor.GREEN))
        linewise = 1
        self.logfile_handle = open(self.logfile_path, 'w+', linewise)
        return self

    def __exit__(self, *ignored):
        """ Closes the log file. """
        print(termcolor.colorize('[INFO]: Close log file.', termcolor.GREEN))
        self.logfile_handle.close()

    def info(self, str):
        """ Prints a message as info to the log file and the console. """
        print(termcolor.colorize('[INFO]: ' + str, termcolor.GREEN))
        self.logfile_handle.write('[INFO]: ' + str + '\n')

    def warning(self, str):
        """ Prints a message as warning to the log file and the console. """
        print(termcolor.colorize('[WARN]: ' + str, termcolor.RED))
        self.logfile_handle.write('[WARN]: ' + str + '\n')

    def failure(self, str):
        """ Prints a message as failure to the log file and the console. """
        print(termcolor.colorize('[FAIL]: ' + str, termcolor.RED,
                                 termcolor.BOLD))
        self.logfile_handle.write('[FAIL]: ' + str + '\n')

    def execute(self, cmd, out_file=sys.stdout, sink=NONE):
        """ Executes a command in the current path and writes the output to the
            log file. """
        if sink is STDOUT or sink is BOTH:
            stdout_file = out_file
        else:
            stdout_file = self.logfile_handle

        if sink is STDERR or sink is BOTH:
            stderr_file = out_file
        else:
            stderr_file = self.logfile_handle

        process = subprocess.Popen(cmd, stdout=stdout_file, stderr=stderr_file)
        process.wait()
