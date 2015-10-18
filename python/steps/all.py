#!/usr/bin/env python3

import steps.clean
import steps.build
import steps.package
import steps.test
import steps.update


class AllStep(object):
    """ Runs all steps to generate deployment packages. """

    def __init__(self):
        self.cleanstep = steps.clean.CleanStep()
        self.buildstep = steps.build.BuildStep()
        self.packagestep = steps.package.PackageStep()
        self.teststep = steps.test.TestStep()
        self.updatestep = steps.update.UpdateStep()

    def execute(self, args, path):
        self.cleanstep.execute(args, path)
        self.updatestep.execute(args, path)
        self.buildstep.execute(args, path)
        self.teststep.execute(args, path)
        self.packagestep.execute(args, path)

    def name(self):
        return 'all'

    def help(self):
        return 'builds all steps to make a package'
