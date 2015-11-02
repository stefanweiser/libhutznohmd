#!/usr/bin/env python3

''' contains functionality to colorize output text '''

import sys

NORMAL, BOLD = range(2)
BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)


def has_colors(stream):
    ''' checks if a stream has support to color the output '''

    if not hasattr(stream, 'isatty'):
        return False
    if not stream.isatty():
        return False

    import curses
    curses.setupterm()
    return curses.tigetnum('colors') > 2


has_colors = has_colors(sys.stdout)


def colorize(text, color=WHITE, boldness=NORMAL):
    ''' colorizes a string '''

    if has_colors:
        return '\x1b[{};{}m'.format(boldness, 30 + color) + text + '\x1b[0m'
    else:
        return text
