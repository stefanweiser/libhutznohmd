#!/usr/bin/env python3

# include system modules
import os
import sys

# add the python subdirectory to the search path
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                'python'))

# include own modules
import logger
import main
import paths

if __name__ == "__main__":
    # first of all generate a structure with all necessary paths
    path = paths.Paths(os.path.dirname(os.path.realpath(__file__)))

    # change into project path
    os.chdir(os.path.dirname(__file__))

    # create build path if not exist
    os.makedirs(path.build, exist_ok=True)

    # create a logger
    with logger.Logger(os.path.join(path.build, 'build.log')) as log_obj:
        # the script requires at least python 3.2
        if sys.version_info < (3, 2):
            log_obj.failure('At least python 3.2 expected, but found:\n' +
                            'python ' + sys.version)
        else:
            # call the real main
            main.main(path, log_obj)
