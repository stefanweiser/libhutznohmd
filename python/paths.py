#!/usr/bin/env python3

from os import makedirs, remove
from os.path import exists
from shutil import rmtree


def renew_folder(path):
    if exists(path):
        rmtree(path)
    makedirs(path)


def renew_file(path):
    if exists(path):
        remove(path)
    open(path, 'a').close()
