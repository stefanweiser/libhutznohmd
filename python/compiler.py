#!/usr/bin/env python3

from os import devnull
from re import compile, MULTILINE
from subprocess import DEVNULL, PIPE, Popen


def get_cxx11_release_include_list():
    process = Popen(['g++', '-std=c++11', '-DNDEBUG', '-E', '-x', 'c++',
                     devnull, '-v'], stdout=DEVNULL, stderr=PIPE)
    output = process.communicate()[1].decode('utf8')
    result = compile(r'^ (.*)$', MULTILINE).findall(output)
    result.pop(0)
    return result


def write_cxx11_release_defines(filename):
    output_file = open(filename, 'w')
    process = Popen(['g++', '-std=c++11', '-DNDEBUG', '-dM', '-E', '-x', 'c++',
                     devnull], stdout=output_file)
    process.wait()
    output_file.close()
