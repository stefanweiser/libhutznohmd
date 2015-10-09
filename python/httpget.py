#!/usr/bin/env python3

import urllib.error
import urllib.request


def http_get(url, filename, attempts=3):
    while attempts > 0:
        try:
            content = urllib.request.urlopen(url, timeout=3).read()
            f = open(filename, 'b+w')
            f.write(content)
            f.close()
            attempts = 0
        except urllib.error.URLError:
            attempts -= 1
