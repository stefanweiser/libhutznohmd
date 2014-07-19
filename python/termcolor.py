#!/usr/bin/env python3

import sys

BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)


def has_colours(stream):
    if not hasattr(stream, "isatty"):
        return False
    if not stream.isatty():
        return False
    try:
        import curses
        curses.setupterm()
        return curses.tigetnum("colors") > 2
    except:
        return False
has_colours = has_colours(sys.stdout)


def colorize(text, color=WHITE):
        if has_colours:
                return ("\x1b[1;%dm" % (30 + color)) + text + "\x1b[0m"
        else:
                return text
