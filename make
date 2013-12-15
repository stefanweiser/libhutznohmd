#!/bin/sh -e

# Defining to script path.
SCRIPTPATH="$(readlink -f $(dirname $0))"

# Cleaning the build.
"$SCRIPTPATH/clean"

# Preparing the build.
"$SCRIPTPATH/prepare"

# Making the build.
cd "$SCRIPTPATH/build"
make -j"$(grep processor /proc/cpuinfo | wc -l)" all
make install
make package
make doc

# Testing the build.
"$SCRIPTPATH/test"
