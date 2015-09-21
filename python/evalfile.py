#!/usr/bin/env python3

from os import getenv
from re import match, sub


def eval_line(line):
    go_on = True
    result = line
    while go_on:
        go_on = False
        m = match('[^$]*\${([_a-zA-Z][_a-zA-Z0-9]*)}.*', result)
        if m is not None:
            for g in m.groups():
                result = sub('(\${' + g + '})', getenv(g, ''), result)
                go_on = True
    return result


def eval_file(input_filename, output_filename):
    with open(input_filename) as f:
        g = open(output_filename, 'w')
        for line in f.readlines():
            g.write(eval_line(line))
        g.close()
