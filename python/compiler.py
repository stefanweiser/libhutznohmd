#!/usr/bin/env python3

''' contains compiler related functions '''

import os
import re
import subprocess


def get_include_list(additional_arguments):
    ''' returns a list of include paths of g++ '''

    process = subprocess.Popen(['g++'] + additional_arguments +
                               ['-E', '-x', 'c++', os.devnull, '-v'],
                               stdout=subprocess.DEVNULL,
                               stderr=subprocess.PIPE)

    # retrieving output
    output = process.communicate()[1].decode('utf8')

    # find all lines starting with a space and skip the first one
    result = re.compile(r'^ (.*)$', re.MULTILINE).findall(output)
    result.pop(0)

    # this is the default include list of g++
    return result


def write_defines(filename, additional_arguments):
    ''' writes all g++'s standard defines into a file '''

    # write them directly into file
    output_file = open(filename, 'w')
    process = subprocess.Popen(['g++'] + additional_arguments +
                               ['-dM', '-E', '-x', 'c++', os.devnull],
                               stdout=output_file)
    process.wait()
    output_file.close()
