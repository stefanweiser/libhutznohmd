#!/usr/bin/env python3

import termcolor


class Logger:
    """ Sets the path to the log file, but actually does not open it. """
    def __init__(self, logfile_path):
        self.logfile_path = logfile_path
        print(termcolor.colorize('[INFO]: Write log to ' + self.logfile_path,
                                 termcolor.GREEN))

    """ Opens the log file. """
    def __enter__(self):
        print(termcolor.colorize('[INFO]: Open log file.', termcolor.GREEN))
        self.logfile_handle = open(self.logfile_path, 'w')

    """ Closes the log file. """
    def __exit__(self, exc_type, exc_value, traceback):
        print(termcolor.colorize('[INFO]: Close log file.', termcolor.GREEN))
        self.logfile_handle.close()

    def info(self, str):
        print(termcolor.colorize('[INFO]: ' + str, termcolor.GREEN))
        self.logfile_handle.write('[INFO]: ' + str + '\n')

    def warning(self, str):
        print(termcolor.colorize('[WARN]: ' + str, termcolor.RED))
        self.logfile_handle.write('[WARN]: ' + str + '\n')

    def failure(self, str):
        print(termcolor.colorize('[FAIL]: ' + str, termcolor.RED,
                                 termcolor.BOLD))
        self.logfile_handle.write('[FAIL]: ' + str + '\n')
