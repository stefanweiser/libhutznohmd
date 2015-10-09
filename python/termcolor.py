#!/usr/bin/env python3

from sys import stdout

NORMAL, BOLD = range(2)
BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)


def has_colours(stream):
    if not hasattr(stream, 'isatty'):
        return False
    if not stream.isatty():
        return False
    try:
        import curses
        curses.setupterm()
        return curses.tigetnum('colors') > 2
    except:
        return False
has_colours = has_colours(stdout)


def colorize(text, color=WHITE, boldness=NORMAL):
        if has_colours:
            return '\x1b[{};{}m'.format(boldness, (30 + color)) + text + \
                   '\x1b[0m'
        else:
            return text
