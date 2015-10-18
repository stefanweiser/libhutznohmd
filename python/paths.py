#!/usr/bin/env python3

from os import makedirs
from os.path import exists
from shutil import rmtree


def renew_folder(path):
    if exists(path):
        rmtree(path)
    makedirs(path)
