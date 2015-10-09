#!/usr/bin/env python3

from os import devnull
from os.path import exists
from re import DOTALL, match
from subprocess import DEVNULL, PIPE, Popen


def get_cxx11_release_include_list():
    process = Popen(['g++', '-std=c++11', '-DNDEBUG', '-E', '-x', 'c++',
                     devnull, '-v'], stdout=DEVNULL, stderr=PIPE)
    output = process.communicate()[1].decode('ascii')
    pathGroups = match('.*#include \"\.\.\.\" search starts here:(.*)' +
                       '#include <\.\.\.> search starts here:(.*)' +
                       'End of search list\..*', output, DOTALL)

    result = list()
    for paths in (pathGroups.group(1), pathGroups.group(2)):
        for path in paths.split():
            if exists(path):
                result.append(path.strip())

    return result


def write_cxx11_release_defines(filename):
    output_file = open(filename, 'w')
    process = Popen(['g++', '-std=c++11', '-DNDEBUG', '-dM', '-E', '-x', 'c++',
                     devnull], stdout=output_file)
    process.wait()
    output_file.close()
