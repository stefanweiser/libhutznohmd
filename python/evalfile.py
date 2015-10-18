#!/usr/bin/env python3

import os
import re


def eval_line(line):
    ''' evaluates a line using the environment variables '''

    # loop till no further replacement string is found
    go_on = True
    result = line
    while go_on:
        go_on = False

        # a replacement string is e.g. ${PWD}
        m = re.match('[^$]*\${([_a-zA-Z][_a-zA-Z0-9]*)}.*', result)
        if m is not None:
            for g in m.groups():
                # replace this string, which itself could contain a replacement
                # string, that gets replaced in the next iteration
                result = re.sub('(\${' + g + '})', os.getenv(g, ''), result)
                go_on = True

    # return the completely "replaced" string
    return result


def eval_file(input_filename, output_filename):
    ''' evaluates a whole file using the environment variables '''

    # do this for the whole file linewise
    with open(input_filename) as f:
        g = open(output_filename, 'w')
        for line in f.readlines():
            g.write(eval_line(line))
        g.close()
