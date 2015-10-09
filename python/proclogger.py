#!/usr/bin/env python3

from subprocess import Popen


def log_process(arguments, working_path, stdout_file, stderr_file):
    process = Popen(arguments, cwd=working_path, stdout=stdout_file,
                    stderr=stderr_file)
    process.wait()
