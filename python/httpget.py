#!/usr/bin/env python3

''' performs HTTP requests '''

import urllib


def http_get(url, filename, attempts=3):
    ''' performs a HTTP get request and writes the result into a file '''

    # unless there are remaining attempts
    while attempts > 0:
        try:
            content = urllib.request.urlopen(url, timeout=3).read()
            f = open(filename, 'w+b')
            f.write(content)
            f.close()
            attempts = 0
        except urllib.error.URLError:
            # any error means: try again
            attempts -= 1
