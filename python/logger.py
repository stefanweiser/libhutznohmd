#!/usr/bin/env python3

import os
import subprocess
import sys
import termcolor


NONE, STDOUT, STDERR, BOTH = range(4)


class Logger(object):
    ''' provides functions to print out data to log file '''

    def __init__(self, logfile_path):
        ''' sets the path to the log file, but actually does not open it '''
        working_dir = os.path.dirname(logfile_path)
        assert(os.path.exists(working_dir))

        self.logfile_path = logfile_path
        self.working_dir = working_dir
        print(termcolor.colorize('[INFO]: Write log to ' + self.logfile_path,
                                 termcolor.GREEN))

    def __enter__(self):
        ''' opens the log file '''
        linewise = 1
        self.logfile_handle = open(self.logfile_path, 'w+', linewise)
        return self

    def __exit__(self, *ignored):
        ''' closes the log file '''
        self.logfile_handle.close()

    def info(self, str):
        ''' prints a message as info to the log file and the console '''
        print(termcolor.colorize('> ' + str, termcolor.GREEN))
        self.logfile_handle.write('[INFO]: ' + str + '\n')

    def warning(self, str):
        ''' prints a message as warning to the log file and the console '''
        print(termcolor.colorize('[WARN]: ' + str, termcolor.RED))
        self.logfile_handle.write('[WARN]: ' + str + '\n')

    def failure(self, str):
        ''' prints a message as failure to the log file and the console '''
        print(termcolor.colorize('[FAIL]: ' + str, termcolor.RED,
                                 termcolor.BOLD))
        self.logfile_handle.write('[FAIL]: ' + str + '\n')

    def execute(self, cmd, out_file=sys.stdout, sink=NONE, working_dir=None):
        ''' executes a command in the current path and writes the output to the
            log file '''

        # default is directory of log file
        if working_dir is None:
            working_dir = self.working_dir

        # stdout gets redirected into file
        if sink is STDOUT or sink is BOTH:
            stdout_file = out_file
        else:
            stdout_file = self.logfile_handle

        # stderr gets redirected into file
        if sink is STDERR or sink is BOTH:
            stderr_file = out_file
        else:
            stderr_file = self.logfile_handle

        # run the command and wait till its end
        process = subprocess.Popen(cmd, cwd=working_dir, stdout=stdout_file,
                                   stderr=stderr_file)
        process.wait()
