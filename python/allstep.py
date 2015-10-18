#!/usr/bin/env python3

import cleanstep
import buildstep
import packagestep
import teststep
import updatestep


class AllStep(object):
    """ Runs all steps to generate deployment packages. """

    def __init__(self):
        self.cleanstep = cleanstep.CleanStep()
        self.buildstep = buildstep.BuildStep()
        self.packagestep = packagestep.PackageStep()
        self.teststep = teststep.TestStep()
        self.updatestep = updatestep.UpdateStep()

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
